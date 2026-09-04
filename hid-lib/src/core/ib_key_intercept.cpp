// ib_key_intercept.cpp — Temporary key interception with queue & replay
// Intercepts physical keyboard input during critical sections (e.g.
// mouse_move_relative -> key_press -> mouse_move_relative sequence).
// Keys are queued and replayed once the critical section ends.
// Uses a lock-free ring buffer so the hook path is zero-lock, zero-alloc
// (~1ms high-frequency trigger friendly).
#include <windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <array>
#include <algorithm>
#include <condition_variable>

#include "../include/hid_controller.h"
#include "key_intercept_internal.h"

// ==================== Programmatic send tracking ====================
// Ring buffer: (key, timestamp_ms). When the Logitech driver sends a key,
// it is recorded here. The LL hook checks this to distinguish our own
// programmatic output from physical input.

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

// ==================== VK -> KeyCode (USB HID usage ID) ====================

static KeyCode vk_to_keycode(uint16_t vk) {
    if (vk >= 'A' && vk <= 'Z')
        return static_cast<KeyCode>(vk - 'A' + 0x04);
    if (vk >= '0' && vk <= '9')
        return static_cast<KeyCode>(vk - '0' + 0x1E);
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

// ==================== Intercept ring buffer ====================
// Lock-free single-producer (hook thread) / single-consumer (main thread)

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

static HHOOK g_keyboard_hook = nullptr;
static std::thread g_hook_thread;
static DWORD g_hook_thread_id = 0;
static std::atomic<bool> g_hook_running{false};
static std::atomic<bool> g_hook_ready{false};
static std::mutex g_hook_start_mutex;
static std::condition_variable g_hook_start_cv;

static LRESULT CALLBACK ll_keyboard_proc(int code, WPARAM wParam, LPARAM lParam) {
    if (code < HC_ACTION)
        return CallNextHookEx(nullptr, code, wParam, lParam);

    // 不在拦截窗口内 -> 放行
    if (!g_intercept_enabled.load(std::memory_order_relaxed))
        return CallNextHookEx(nullptr, code, wParam, lParam);

    KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    uint16_t vk = static_cast<uint16_t>(kb->vkCode);
    if (vk == 0)
        return CallNextHookEx(nullptr, code, wParam, lParam);

    KeyCode kc = vk_to_keycode(vk);

    // 跳过自己的程序发送
    if (was_recently_sent(kc))
        return CallNextHookEx(nullptr, code, wParam, lParam);

    // 入队：lock-free ring buffer
    if (g_intercept_count.load(std::memory_order_relaxed) < kInterceptQueueSize) {
        int idx = g_intercept_head.fetch_add(1, std::memory_order_relaxed) % kInterceptQueueSize;
        g_intercept_queue[idx] = {kc, (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)};
        g_intercept_count.fetch_add(1, std::memory_order_release);
    }

    return 1; // 拦截
}

// ==================== Hook message loop ====================

static void hook_message_loop() {
    g_hook_thread_id = GetCurrentThreadId();

    HMODULE hmod_dll = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                      (LPCTSTR)ll_keyboard_proc, &hmod_dll);
    g_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, ll_keyboard_proc,
                                       hmod_dll, 0);

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

// ==================== Hook lifecycle ====================

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
        std::lock_guard<std::mutex> lock(g_recent_send_mutex);
        g_recent_send_idx = 0;
    }
}

// ==================== Public API ====================

DLLAPI void WINAPI begin_key_intercept() {
    start_hook();
    g_intercept_enabled.store(true, std::memory_order_release);
}

DLLAPI void WINAPI end_key_intercept() {
    g_intercept_enabled.store(false, std::memory_order_release);

    // 按入队顺序重放所有按键
    int count = g_intercept_count.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        InterceptEntry& ev = g_intercept_queue[i];
        if (ev.is_down)
            key_down(ev.code);
        else
            key_up(ev.code);
    }

    // 重置队列
    g_intercept_head.store(0, std::memory_order_relaxed);
    g_intercept_tail = 0;
    g_intercept_count.store(0, std::memory_order_release);

    stop_hook();
}

DLLAPI void WINAPI discard_queued_keys() {
    g_intercept_enabled.store(false, std::memory_order_release);
    g_intercept_head.store(0, std::memory_order_relaxed);
    g_intercept_tail = 0;
    g_intercept_count.store(0, std::memory_order_relaxed);
    stop_hook();
}
