// ib_key_intercept.cpp — Key interception & remapping via WH_KEYBOARD_LL
// Intercepts physical keyboard input and remaps keys using the Logitech HID driver.
// Blocked keys never reach any application window.
// Programmatic sends (via Logitech driver) are tracked via a timestamped ring buffer
// and NOT intercepted, so the remap loop only applies to real physical key presses.
#include <windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <set>
#include <array>
#include <algorithm>
#include <condition_variable>

#include "../include/hid_controller.h"
#include "key_intercept_internal.h"

// ==================== Global state ====================

static std::atomic<bool> g_intercept_enabled{false};    // remap active
static std::atomic<bool> g_forbid_all_keys{false};      // block-all active
static std::atomic<bool> g_remap_output_enabled{false}; // allow remap output

static std::unordered_map<KeyCode, KeyCode> g_remap_table; // from → to
static std::mutex g_remap_mutex;

static HHOOK g_keyboard_hook = nullptr;
static std::thread g_hook_thread;
static DWORD g_hook_thread_id = 0;
static std::atomic<bool> g_hook_running{false};
static std::atomic<bool> g_hook_ready{false};
static std::mutex g_hook_start_mutex;
static std::condition_variable g_hook_start_cv;

// Track which remapped keys are currently held down (for cleanup on disable)
static std::set<KeyCode> g_active_remaps;
static std::mutex g_active_remaps_mutex;

// Whether the hook should be running (shared between remap and forbid-all)
static bool should_hook_run() {
    return g_intercept_enabled.load() || g_forbid_all_keys.load();
}

// ==================== Programmatic send tracking ====================
// Ring buffer: (key, timestamp_ms).  When the Logitech driver sends a key,
// it is recorded here.  The LL hook checks whether an incoming key was
// recently sent by our own code — if so, the key passes through.
// This is necessary because WH_KEYBOARD_LL fires on a different thread
// (the hook message pump), so a simple atomic flag would have a race window.

static constexpr int kRecentSendCapacity = 128;
static std::array<std::pair<KeyCode, DWORD>, kRecentSendCapacity> g_recent_sends;
static int g_recent_send_idx{0};
static std::mutex g_recent_send_mutex;

void on_programmatic_keyboard_send(KeyCode vk) {
    std::lock_guard<std::mutex> lock(g_recent_send_mutex);
    g_recent_sends[g_recent_send_idx % kRecentSendCapacity] = {vk, GetTickCount()};
    ++g_recent_send_idx;
}

static bool was_recently_sent(KeyCode vk) {
    std::lock_guard<std::mutex> lock(g_recent_send_mutex);
    DWORD now = GetTickCount();
    int count = (std::min)(g_recent_send_idx, kRecentSendCapacity);
    for (int i = 0; i < count; ++i) {
        int idx = (g_recent_send_idx - 1 - i) % kRecentSendCapacity;
        auto& entry = g_recent_sends[idx];
        if (entry.first == vk && (now - entry.second) < 100) {
            return true;
        }
    }
    return false;
}

// ==================== VK → KeyCode (USB HID usage ID) ====================

static KeyCode vk_to_keycode(uint16_t vk) {
    // A-Z: VK 0x41-0x5A → HID 0x04-0x1D
    if (vk >= 'A' && vk <= 'Z')
        return static_cast<KeyCode>(vk - 'A' + 0x04);

    // 0-9 top row: VK 0x30-0x39 → HID 0x1E-0x27
    if (vk >= '0' && vk <= '9')
        return static_cast<KeyCode>(vk - '0' + 0x1E);

    // F1-F24: VK 0x70-0x87 → HID 0x3A-0x4D
    if (vk >= VK_F1 && vk <= VK_F24)
        return static_cast<KeyCode>(vk - VK_F1 + static_cast<uint16_t>(KeyCode::F1));

    switch (vk) {
    case VK_BACK:      return KeyCode::BACK;
    case VK_TAB:       return KeyCode::TAB;
    case VK_RETURN:    return KeyCode::ENTER;
    case VK_SHIFT:     return KeyCode::SHIFT;
    case VK_CONTROL:   return KeyCode::CTRL;
    case VK_MENU:      return KeyCode::ALT;
    case VK_PAUSE:     return KeyCode::PAUSE;
    case VK_CAPITAL:   return KeyCode::CAPS_LOCK;
    case VK_ESCAPE:    return KeyCode::ESCAPE;
    case VK_SPACE:     return KeyCode::SPACE;
    case VK_PRIOR:     return KeyCode::PRIOR;
    case VK_NEXT:      return KeyCode::NEXT;
    case VK_END:       return KeyCode::END;
    case VK_HOME:      return KeyCode::HOME;
    case VK_LEFT:      return KeyCode::LEFT;
    case VK_UP:        return KeyCode::UP;
    case VK_RIGHT:     return KeyCode::RIGHT;
    case VK_DOWN:      return KeyCode::DOWN;
    case VK_SNAPSHOT:  return KeyCode::SNAPSHOT;
    case VK_INSERT:    return KeyCode::INSERT;
    case VK_DELETE:    return KeyCode::DEL;
    case VK_LWIN:      return KeyCode::LWIN;
    case VK_RWIN:      return KeyCode::RWIN;
    case VK_APPS:      return KeyCode::APPS;
    case VK_NUMPAD0:   return KeyCode::NUMPAD0;
    case VK_NUMPAD1:   return KeyCode::NUMPAD1;
    case VK_NUMPAD2:   return KeyCode::NUMPAD2;
    case VK_NUMPAD3:   return KeyCode::NUMPAD3;
    case VK_NUMPAD4:   return KeyCode::NUMPAD4;
    case VK_NUMPAD5:   return KeyCode::NUMPAD5;
    case VK_NUMPAD6:   return KeyCode::NUMPAD6;
    case VK_NUMPAD7:   return KeyCode::NUMPAD7;
    case VK_NUMPAD8:   return KeyCode::NUMPAD8;
    case VK_NUMPAD9:   return KeyCode::NUMPAD9;
    case VK_MULTIPLY:  return KeyCode::MULTIPLY;
    case VK_ADD:       return KeyCode::ADD;
    case VK_SUBTRACT:  return KeyCode::SUBTRACT;
    case VK_DECIMAL:   return KeyCode::DECIMAL;
    case VK_DIVIDE:    return KeyCode::DIVIDE;
    case VK_NUMLOCK:   return KeyCode::NUM_LOCK;
    case VK_SCROLL:    return KeyCode::SCROLL_LOCK;
    case VK_LSHIFT:    return KeyCode::LSHIFT;
    case VK_RSHIFT:    return KeyCode::RSHIFT;
    case VK_LCONTROL:  return KeyCode::LCTRL;
    case VK_RCONTROL:  return KeyCode::RCTRL;
    case VK_LMENU:     return KeyCode::LALT;
    case VK_RMENU:     return KeyCode::RALT;
    case VK_OEM_1:     return KeyCode::OEM_1;
    case VK_OEM_PLUS:  return KeyCode::OEM_PLUS;
    case VK_OEM_COMMA: return KeyCode::OEM_COMMA;
    case VK_OEM_MINUS: return KeyCode::OEM_MINUS;
    case VK_OEM_PERIOD:return KeyCode::OEM_PERIOD;
    case VK_OEM_2:     return KeyCode::OEM_2;
    case VK_OEM_3:     return KeyCode::OEM_3;
    case VK_OEM_4:     return KeyCode::OEM_4;
    case VK_OEM_5:     return KeyCode::OEM_5;
    case VK_OEM_6:     return KeyCode::OEM_6;
    case VK_OEM_7:     return KeyCode::OEM_7;
    case VK_OEM_102:   return KeyCode::OEM_102;
    }
    return static_cast<KeyCode>(0);
}

// ==================== WH_KEYBOARD_LL hook ====================

static LRESULT CALLBACK ll_keyboard_proc(int code, WPARAM wParam, LPARAM lParam) {
    if (code >= HC_ACTION && (g_intercept_enabled.load() || g_forbid_all_keys.load())) {
        KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        bool keydown  = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool keyup    = (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP);

        if ((keydown || keyup) && (kb->vkCode != 0)) {
            KeyCode vk = vk_to_keycode(static_cast<uint16_t>(kb->vkCode));

            // Skip keys that our own Logitech driver just sent.
            // This prevents the remapped output from being intercepted again.
            if (!was_recently_sent(vk)) {
                // 1) Check remap table
                {
                    std::lock_guard<std::mutex> lock(g_remap_mutex);
                    auto it = g_remap_table.find(vk);
                    if (it != g_remap_table.end()) {
                        if (keydown && g_remap_output_enabled.load()) {
                            key_down(it->second);
                            {
                                std::lock_guard<std::mutex> lock2(g_active_remaps_mutex);
                                g_active_remaps.insert(it->second);
                            }
                        }
                        else if (keyup) {
                            // Only send key-up if key-down was actually sent
                            // (i.e. remap output was enabled when this key was pressed)
                            bool was_active = false;
                            {
                                std::lock_guard<std::mutex> lock2(g_active_remaps_mutex);
                                auto it2 = g_active_remaps.find(it->second);
                                if (it2 != g_active_remaps.end()) {
                                    was_active = true;
                                    g_active_remaps.erase(it2);
                                }
                            }
                            if (was_active) {
                                key_up(it->second);
                            }
                        }
                        return 1; // Block original key from reaching any app
                    }
                }

                // 2) Forbid-all mode: block every physical key
                if (g_forbid_all_keys.load()) {
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// ==================== Hook thread ====================

static void hook_message_loop() {
    g_hook_thread_id = GetCurrentThreadId();

    g_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, ll_keyboard_proc,
                                       GetModuleHandle(nullptr), 0);

    // Signal the caller that the hook is ready (or failed)
    g_hook_ready.store(true);
    g_hook_start_cv.notify_one();

    if (!g_keyboard_hook) {
        g_hook_running.store(false);
        return;
    }

    MSG msg;
    while (g_hook_running.load() && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_keyboard_hook) {
        UnhookWindowsHookEx(g_keyboard_hook);
        g_keyboard_hook = nullptr;
    }
    g_hook_running.store(false);
}

// ==================== Hook lifecycle helpers ====================

static void start_hook() {
    if (g_hook_running.load()) return;
    g_hook_ready.store(false);
    g_hook_running.store(true);
    g_hook_thread = std::thread(hook_message_loop);
    {
        std::unique_lock<std::mutex> lock(g_hook_start_mutex);
        g_hook_start_cv.wait(lock, [] { return g_hook_ready.load(); });
    }
}

static void stop_hook() {
    if (!g_hook_running.load()) return;
    g_hook_running.store(false);
    if (g_keyboard_hook) {
        PostThreadMessage(g_hook_thread_id, WM_QUIT, 0, 0);
    }
    if (g_hook_thread.joinable()) {
        g_hook_thread.join();
    }
    g_hook_thread_id = 0;

    // Release any remapped keys still held down
    std::lock_guard<std::mutex> lock(g_active_remaps_mutex);
    for (auto kc : g_active_remaps) {
        key_up(kc);
    }
    g_active_remaps.clear();

    // Clear programmatic send tracking
    {
        std::lock_guard<std::mutex> lock_recent(g_recent_send_mutex);
        g_recent_send_idx = 0;
    }
}

// ==================== Public API ====================

DLLAPI bool WINAPI register_key_remap(KeyCode from_key, KeyCode to_key) {
    if (from_key == to_key) return false;
    std::lock_guard<std::mutex> lock(g_remap_mutex);
    g_remap_table[from_key] = to_key;
    return true;
}

DLLAPI bool WINAPI unregister_key_remap(KeyCode from_key) {
    std::lock_guard<std::mutex> lock(g_remap_mutex);
    return g_remap_table.erase(from_key) > 0;
}

DLLAPI void WINAPI clear_key_remaps() {
    std::lock_guard<std::mutex> lock(g_remap_mutex);
    g_remap_table.clear();
}

DLLAPI bool WINAPI set_intercept_device(uintptr_t device_handle) {
    (void)device_handle;
    return true;
}

DLLAPI void WINAPI enable_key_intercept(bool enable) {
    bool was_enabled = g_intercept_enabled.exchange(enable);
    if (enable && !was_enabled) {
        start_hook();
    }
    else if (!enable && was_enabled && !should_hook_run()) {
        stop_hook();
    }
}

DLLAPI bool WINAPI start_forbid_keys(uintptr_t device_handle) {
    g_forbid_all_keys.store(true);
    set_intercept_device(device_handle);
    start_hook();
    return true;
}

DLLAPI void WINAPI stop_forbid_keys() {
    g_forbid_all_keys.store(false);
    if (!should_hook_run()) {
        stop_hook();
    }
}

DLLAPI void WINAPI enable_remap_output() {
    g_remap_output_enabled.store(true);
}

DLLAPI void WINAPI disable_remap_output() {
    g_remap_output_enabled.store(false);
}
