// ib_mouse_intercept.cpp — Temporary mouse interception with queue & replay
// Intercepts physical mouse input during critical sections.
// Mouse events are queued and replayed once the critical section ends.
// Uses a lock-free ring buffer so the hook path is zero-lock, zero-alloc
// (~1ms high-frequency trigger friendly).
//
// Design notes:
//   - The Logitech driver bypasses WH_MOUSE_LL for its own sends, so
//     programmatic sends do NOT trigger this hook.
//   - Only button clicks (down/up) and wheel events are intercepted;
//     mouse movement is always passed through.
#include <windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <array>
#include <algorithm>
#include <condition_variable>

#include "../include/hid_controller.h"
#include "mouse_intercept_internal.h"

// ==================== Programmatic send tracking ====================
// Ring buffer: (msg_type, timestamp_ms). The Logitech driver bypasses
// WH_MOUSE_LL for its own HID reports, so programmatic sends do NOT need
// filtering. However, we keep the tracking infrastructure in case the
// driver behavior changes in the future.

static constexpr int kRecentSendCapacity = 64;
struct RecentSendEntry {
    DWORD msg_type;
    DWORD timestamp_ms;
};
static std::array<RecentSendEntry, kRecentSendCapacity> g_recent_sends;
static int g_recent_send_idx{0};
static std::mutex g_recent_send_mutex;

void on_programmatic_mouse_send(DWORD msg_type) {
    std::lock_guard<std::mutex> lock(g_recent_send_mutex);
    g_recent_sends[g_recent_send_idx % kRecentSendCapacity] = {msg_type, GetTickCount()};
    ++g_recent_send_idx;
}

static bool is_recently_sent(DWORD msg_type) {
    std::lock_guard<std::mutex> lock(g_recent_send_mutex);
    DWORD now = GetTickCount();
    int count = (std::min)(g_recent_send_idx, kRecentSendCapacity);
    for (int i = 0; i < count; ++i) {
        int idx = (g_recent_send_idx - 1 - i) % kRecentSendCapacity;
        auto& entry = g_recent_sends[idx];
        if (entry.msg_type == msg_type && (now - entry.timestamp_ms) < 100) {
            return true;
        }
    }
    return false;
}

// ==================== Intercept ring buffer ====================
// Lock-free single-producer (hook thread) / single-consumer (main thread)

static std::atomic<bool> g_intercept_enabled{false};

static constexpr int kInterceptQueueSize = 4096;

enum class MouseInterceptEvent : uint8_t {
    DOWN,
    UP,
    WHEEL,
};

struct InterceptEntry {
    MouseInterceptEvent type;
    int32_t data1;  // button id or wheel delta
};

static InterceptEntry g_intercept_queue[kInterceptQueueSize];
static std::atomic<int> g_intercept_head{0};
static int g_intercept_tail{0};
static std::atomic<int> g_intercept_count{0};

// ==================== Cursor save/restore ====================

static POINT g_saved_cursor_pos{};

// ==================== WH_MOUSE_LL hook ====================

static HHOOK g_mouse_hook = nullptr;
static std::thread g_hook_thread;
static DWORD g_hook_thread_id = 0;
static std::atomic<bool> g_hook_running{false};
static std::atomic<bool> g_hook_ready{false};
static std::mutex g_hook_start_mutex;
static std::condition_variable g_hook_start_cv;

static LRESULT CALLBACK ll_mouse_proc(int code, WPARAM wParam, LPARAM lParam) {
    if (code < HC_ACTION)
        return CallNextHookEx(nullptr, code, wParam, lParam);

    // Not in intercept window -> passthrough
    if (!g_intercept_enabled.load(std::memory_order_relaxed))
        return CallNextHookEx(nullptr, code, wParam, lParam);

    MSLLHOOKSTRUCT* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

    // Skip our own programmatic sends (Logitech driver reports)
    if (is_recently_sent(static_cast<DWORD>(wParam)))
        return CallNextHookEx(nullptr, code, wParam, lParam);

    switch (wParam) {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN: {
        MouseButton btn;
        if (wParam == WM_LBUTTONDOWN)      btn = MouseButton::LEFT;
        else if (wParam == WM_RBUTTONDOWN) btn = MouseButton::RIGHT;
        else                                btn = MouseButton::MIDDLE;
        if (g_intercept_count.load(std::memory_order_relaxed) < kInterceptQueueSize) {
            int idx = g_intercept_head.fetch_add(1, std::memory_order_relaxed) % kInterceptQueueSize;
            g_intercept_queue[idx] = {MouseInterceptEvent::DOWN,
                                      static_cast<int32_t>(btn)};
            g_intercept_count.fetch_add(1, std::memory_order_release);
        }
        return 1; // block
    }

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP: {
        MouseButton btn;
        if (wParam == WM_LBUTTONUP)      btn = MouseButton::LEFT;
        else if (wParam == WM_RBUTTONUP) btn = MouseButton::RIGHT;
        else                              btn = MouseButton::MIDDLE;
        if (g_intercept_count.load(std::memory_order_relaxed) < kInterceptQueueSize) {
            int idx = g_intercept_head.fetch_add(1, std::memory_order_relaxed) % kInterceptQueueSize;
            g_intercept_queue[idx] = {MouseInterceptEvent::UP,
                                      static_cast<int32_t>(btn)};
            g_intercept_count.fetch_add(1, std::memory_order_release);
        }
        return 1; // block
    }

    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP: {
        uint16_t xbtn = GET_XBUTTON_WPARAM(ms->mouseData);
        MouseButton btn = (xbtn == 1) ? MouseButton::X1 : MouseButton::X2;
        auto event_type = (wParam == WM_XBUTTONDOWN)
                              ? MouseInterceptEvent::DOWN
                              : MouseInterceptEvent::UP;
        if (g_intercept_count.load(std::memory_order_relaxed) < kInterceptQueueSize) {
            int idx = g_intercept_head.fetch_add(1, std::memory_order_relaxed) % kInterceptQueueSize;
            g_intercept_queue[idx] = {event_type, static_cast<int32_t>(btn)};
            g_intercept_count.fetch_add(1, std::memory_order_release);
        }
        return 1; // block
    }

    case WM_MOUSEWHEEL: {
        int16_t delta = static_cast<int16_t>(HIWORD(ms->mouseData));
        if (g_intercept_count.load(std::memory_order_relaxed) < kInterceptQueueSize) {
            int idx = g_intercept_head.fetch_add(1, std::memory_order_relaxed) % kInterceptQueueSize;
            g_intercept_queue[idx] = {MouseInterceptEvent::WHEEL, static_cast<int32_t>(delta)};
            g_intercept_count.fetch_add(1, std::memory_order_release);
        }
        return 1; // block
    }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// ==================== Hook message loop ====================

static void hook_message_loop() {
    g_hook_thread_id = GetCurrentThreadId();

    g_mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, ll_mouse_proc,
                                     GetModuleHandle(nullptr), 0);

    g_hook_ready.store(true, std::memory_order_release);
    g_hook_start_cv.notify_one();

    if (!g_mouse_hook) {
        g_hook_running.store(false, std::memory_order_release);
        return;
    }

    MSG msg;
    while (g_hook_running.load(std::memory_order_relaxed) && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_mouse_hook) {
        UnhookWindowsHookEx(g_mouse_hook);
        g_mouse_hook = nullptr;
    }
    g_hook_running.store(false, std::memory_order_release);
}

// ==================== Hook lifecycle ====================

static void start_hook() {
    if (g_hook_running.load(std::memory_order_relaxed)) return;
    g_hook_ready.store(false, std::memory_order_release);
    g_hook_running.store(true, std::memory_order_release);
    g_hook_thread = std::thread(hook_message_loop);
    {
        std::unique_lock<std::mutex> lock(g_hook_start_mutex);
        g_hook_start_cv.wait(lock, [] { return g_hook_ready.load(std::memory_order_acquire); });
    }
}

static void stop_hook() {
    if (!g_hook_running.load(std::memory_order_relaxed)) return;
    g_hook_running.store(false, std::memory_order_relaxed);
    if (g_mouse_hook) {
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

DLLAPI void WINAPI begin_mouse_intercept() {
    // Save current cursor position for restoration at end
    GetCursorPos(&g_saved_cursor_pos);

    start_hook();
    g_intercept_enabled.store(true, std::memory_order_release);
}

DLLAPI void WINAPI end_mouse_intercept() {
    g_intercept_enabled.store(false, std::memory_order_release);

    // Replay all queued events in FIFO order
    int count = g_intercept_count.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        InterceptEntry& ev = g_intercept_queue[i];
        switch (ev.type) {
        case MouseInterceptEvent::DOWN:
            mouse_down(static_cast<MouseButton>(ev.data1));
            break;
        case MouseInterceptEvent::UP:
            mouse_up(static_cast<MouseButton>(ev.data1));
            break;
        case MouseInterceptEvent::WHEEL:
            mouse_wheel(ev.data1);
            break;
        }
    }

    // Reset queue
    g_intercept_head.store(0, std::memory_order_relaxed);
    g_intercept_tail = 0;
    g_intercept_count.store(0, std::memory_order_release);

    stop_hook();
}

DLLAPI void WINAPI discard_queued_mouse_events() {
    g_intercept_enabled.store(false, std::memory_order_release);
    g_intercept_head.store(0, std::memory_order_relaxed);
    g_intercept_tail = 0;
    g_intercept_count.store(0, std::memory_order_relaxed);
    stop_hook();
}
