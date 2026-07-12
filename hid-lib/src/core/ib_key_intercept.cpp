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
#include <array>
#include <algorithm>
#include <condition_variable>
#include <vector>

#include "../include/hid_controller.h"
#include "key_intercept_internal.h"

// ==================== Device-level key blocking ====================
// Key codes → device handle mapping, updated by raw input callback.
// The LL hook checks this map to determine if a key should be blocked
// based on its source device.

static std::unordered_map<uint16_t, uintptr_t> g_key_device_map; // vk → device_handle
static std::mutex g_key_device_mutex;

static std::unordered_map<uintptr_t, bool> g_device_block; // device → blocked
static std::mutex g_device_block_mutex;

static bool is_key_blocked_by_device(uint16_t vk) {
    uintptr_t dev;
    {
        std::lock_guard<std::mutex> lock(g_key_device_mutex);
        auto it = g_key_device_map.find(vk);
        if (it == g_key_device_map.end()) return false;
        dev = it->second;
    }

    // Device 0 = wildcard: blocks all devices
    std::lock_guard<std::mutex> lock2(g_device_block_mutex);
    if (dev == 0) return g_device_block[0];
    return g_device_block[dev];
}

static void on_key_down(uint16_t vk, uintptr_t device_handle) {
    std::lock_guard<std::mutex> lock(g_key_device_mutex);
    std::lock_guard<std::mutex> lock2(g_device_block_mutex);
    if (!g_device_block[device_handle]) return;
    g_key_device_map[vk] = device_handle;
}

static void on_key_up(uint16_t vk, uintptr_t device_handle) {
    std::lock_guard<std::mutex> lock(g_key_device_mutex);
    auto it = g_key_device_map.find(vk);
    if (it != g_key_device_map.end() && it->second == device_handle) {
        g_key_device_map.erase(it);
    }
}

void fire_key_down(uint16_t vk, uintptr_t device_handle) {
    on_key_down(vk, device_handle);
}

void fire_key_up(uint16_t vk, uintptr_t device_handle) {
    on_key_up(vk, device_handle);
}

static void clear_device_blocking(uintptr_t device_handle) {
    std::lock_guard<std::mutex> lock(g_device_block_mutex);
    g_device_block.erase(device_handle);
    if (device_handle == 0) {
        // Clear all keys from all devices
        g_key_device_map.clear();
    } else {
        // Remove only keys from this device
        auto it = g_key_device_map.begin();
        while (it != g_key_device_map.end()) {
            if (it->second == device_handle) {
                it = g_key_device_map.erase(it);
            } else {
                ++it;
            }
        }
    }
}

// ==================== Global state ====================

static std::unordered_map<KeyCode, KeyCode> g_remap_table; // from → to
static std::mutex g_remap_mutex;

static uintptr_t g_device_handle{0}; // 0 = intercept all devices

static HHOOK g_keyboard_hook = nullptr;
static std::thread g_hook_thread;
static DWORD g_hook_thread_id = 0;
static std::atomic<bool> g_hook_running{false};
static std::atomic<bool> g_hook_ready{false};
static std::mutex g_hook_start_mutex;
static std::condition_variable g_hook_start_cv;
static std::atomic<bool> g_all_keys_blocked{false};

// Whether the hook should be running (auto-managed)
static bool should_hook_run() {
    std::lock_guard<std::mutex> lock(g_remap_mutex);
    return !g_remap_table.empty();
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

// ==================== Delayed remap buffer ====================
// Ring buffer of remapped keys waiting to be flushed.
// Keys are queued on intercept, then sent in bulk on flush_remap_output().
// When full, the oldest entry is evicted (FIFO).

static constexpr int kPendingCapacity = 256;
static std::array<KeyCode, kPendingCapacity> g_pending_remaps;
static int g_pending_count{0};
static int g_pending_head{0};
static std::mutex g_pending_mutex;

static void push_pending(KeyCode kc) {
    std::lock_guard<std::mutex> lock(g_pending_mutex);
    int idx;
    if (g_pending_count >= kPendingCapacity) {
        idx = g_pending_head;
        g_pending_head = (g_pending_head + 1) % kPendingCapacity;
    } else {
        idx = (g_pending_head + g_pending_count) % kPendingCapacity;
        ++g_pending_count;
    }
    g_pending_remaps[idx] = kc;
}

// ==================== 临时拦截 + 排队 + 重放 globals ====================
// hook 线程只写，主线程只读，hook 路径零锁零分配
static std::atomic<bool> g_intercept_enabled{false};

static constexpr int kInterceptQueueSize = 512;
struct InterceptEntry {
    KeyCode code;
    bool is_down;
};
static InterceptEntry g_intercept_queue[kInterceptQueueSize];
static std::atomic<int> g_intercept_head{0};
static int g_intercept_tail{0};
static std::atomic<int> g_intercept_count{0};

// ==================== WH_KEYBOARD_LL hook ====================

static LRESULT CALLBACK ll_keyboard_proc(int code, WPARAM wParam, LPARAM lParam) {
    if (code < HC_ACTION)
        return CallNextHookEx(nullptr, code, wParam, lParam);

    KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

    // 1. 临时拦截优先：atomic load，hook 路径零锁
    if (g_intercept_enabled.load(std::memory_order_relaxed)) {
        uint16_t vk = static_cast<uint16_t>(kb->vkCode);
        if (vk != 0) {
            KeyCode kc = vk_to_keycode(vk);
            if (!was_recently_sent(kc)) {
                // 入队：单写者无锁，release 保证 count++ 前 entry 已写入
                if (g_intercept_count.load(std::memory_order_relaxed) < kInterceptQueueSize) {
                    int idx = g_intercept_head.fetch_add(1, std::memory_order_relaxed) % kInterceptQueueSize;
                    g_intercept_queue[idx] = {kc, (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)};
                    g_intercept_count.fetch_add(1, std::memory_order_release);
                }
                return 1; // 拦截
            }
        }
    }

    // 2. Device-level blocking
    if (g_all_keys_blocked.load() || (g_device_block.empty() == false && is_key_blocked_by_device(static_cast<uint16_t>(kb->vkCode)))) {
        return 1;
    }
    if (!should_hook_run()) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    bool keydown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    bool keyup   = (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP);

    if ((keydown || keyup) && (kb->vkCode != 0)) {
        KeyCode vk = vk_to_keycode(static_cast<uint16_t>(kb->vkCode));

        // Skip keys our own Logitech driver sent (programmatic output)
        if (!was_recently_sent(vk)) {
            std::lock_guard<std::mutex> lock(g_remap_mutex);
            auto it = g_remap_table.find(vk);
            if (it != g_remap_table.end()) {
                if (keydown) {
                    push_pending(it->second);
                }
                return 1; // Block original key from reaching any app
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

    // Clear programmatic send tracking
    {
        std::lock_guard<std::mutex> lock_recent(g_recent_send_mutex);
        g_recent_send_idx = 0;
    }
}

// ==================== Public API ====================

DLLAPI void WINAPI set_intercept_device(uintptr_t device_handle) {
    g_device_handle = device_handle;
}

DLLAPI bool WINAPI register_key_remap(KeyCode from_key, KeyCode to_key) {
    
    bool was_empty;
    {
        std::lock_guard<std::mutex> lock(g_remap_mutex);
        was_empty = g_remap_table.empty();
        g_remap_table[from_key] = to_key;
    }
    if (was_empty) {
        start_hook();
    }
    return true;
}

DLLAPI bool WINAPI unregister_key_remap(KeyCode from_key) {
    bool now_empty = false;
    {
        std::lock_guard<std::mutex> lock(g_remap_mutex);
        if (g_remap_table.erase(from_key) == 0) {
            return false;
        }
        now_empty = g_remap_table.empty();
    }
    if (now_empty) {
        stop_hook();
    }
    return true;
}

DLLAPI void WINAPI clear_key_remaps() {
    {
        std::lock_guard<std::mutex> lock(g_remap_mutex);
        g_remap_table.clear();
    }
    stop_hook();
    clear_pending_remaps();
}

DLLAPI void WINAPI flush_remap_output() {
    std::vector<KeyCode> local;
    {
        std::lock_guard<std::mutex> lock(g_pending_mutex);
        local.reserve(g_pending_count);
        for (int i = 0; i < g_pending_count; ++i) {
            int idx = (g_pending_head + i) % kPendingCapacity;
            local.push_back(g_pending_remaps[idx]);
        }
        g_pending_count = 0;
        g_pending_head = 0;
    }
    for (auto kc : local) {
        key_press(kc);
    }
}

DLLAPI void WINAPI clear_pending_remaps() {
    std::lock_guard<std::mutex> lock(g_pending_mutex);
    g_pending_count = 0;
    g_pending_head = 0;
}

// ==================== Device-level key blocking API ====================

DLLAPI void WINAPI block_key_device(uintptr_t device_handle) {
    {
        std::lock_guard<std::mutex> lock(g_device_block_mutex);
        g_device_block[device_handle] = true;
    }
    // Register all currently pressed keys as belonging to this device
    // so the LL hook can start blocking them.
    std::vector<uint16_t> keys = get_pressed_keys();
    for (auto vk : keys) {
        on_key_down(vk, device_handle);
    }
}

DLLAPI void WINAPI unblock_key_device(uintptr_t device_handle) {
    {
        std::lock_guard<std::mutex> lock(g_device_block_mutex);
        g_device_block[device_handle] = false;
    }
    clear_device_blocking(device_handle);
}

// ==================== 临时拦截 + 排队 + 重放 ====================

DLLAPI void WINAPI begin_key_intercept() {
    start_hook(); // 确保 WH_KEYBOARD_LL 钩子已启动
    g_intercept_enabled.store(true, std::memory_order_release);
}

DLLAPI void WINAPI end_key_intercept() {
    g_intercept_enabled.store(false, std::memory_order_release);

    // acquire 保证读到 count 时所有 entry 已完整写入
    int count = g_intercept_count.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        InterceptEntry& ev = g_intercept_queue[i];
        key_down(ev.code);
        key_up(ev.code);
    }

    // 重置队列
    g_intercept_head.store(0, std::memory_order_relaxed);
    g_intercept_tail = 0;
    g_intercept_count.store(0, std::memory_order_release);

    // 如果没有 remap 注册，停掉 hook 线程
    bool remap_empty;
    { std::lock_guard<std::mutex> lock(g_remap_mutex); remap_empty = g_remap_table.empty(); }
    if (remap_empty) stop_hook();
}

DLLAPI void WINAPI discard_queued_keys() {
    g_intercept_enabled.store(false, std::memory_order_release);
    g_intercept_head.store(0, std::memory_order_relaxed);
    g_intercept_tail = 0;
    g_intercept_count.store(0, std::memory_order_relaxed);

    bool remap_empty;
    { std::lock_guard<std::mutex> lock(g_remap_mutex); remap_empty = g_remap_table.empty(); }
    if (remap_empty) stop_hook();
}
