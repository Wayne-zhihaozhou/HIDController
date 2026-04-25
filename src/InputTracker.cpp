// InputTracker.cpp
// Windows RAW INPUT API 键盘鼠标事件检测
// 基于 WindowsProject2 的 input_tracker.cpp 融合到 HIDController 项目

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <windows.h>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <set>
#include <functional>

namespace py = pybind11;

// ==================== 全局变量 ====================
std::atomic<bool> g_running(false);
std::thread g_workerThread;
HWND g_hwnd = nullptr;

// 回调函数句柄
std::function<void(uintptr_t, long, long)> g_mouse_callback = nullptr;
std::function<void(uintptr_t, uint16_t, bool)> g_key_callback = nullptr;
std::function<void(uintptr_t, uint32_t, bool)> g_mouse_button_callback = nullptr;

// 鼠标位移累加器
std::atomic<long> g_accumulated_dx{0};
std::atomic<long> g_accumulated_dy{0};

// 键盘状态记录
std::mutex g_key_mutex;
std::set<uint16_t> g_pressed_keys; // 记录当前按下的虚拟键码 (VKEY)

// ==================== RAW INPUT 注册 ====================

/**
 * 注册 Raw Input 以监听鼠标和键盘事件
 */
void RegisterRawInput(HWND hwnd) {
    RAWINPUTDEVICE rid[2];

    // 鼠标 - usUsagePage=0x01 (Generic Desktop), usUsage=0x02 (Mouse)
    rid[0].usUsagePage = 0x01;
    rid[0].usUsage = 0x02;
    rid[0].dwFlags = RIDEV_INPUTSINK; // 即使窗口失去焦点也能接收
    rid[0].hwndTarget = hwnd;

    // 键盘 - usUsagePage=0x01 (Generic Desktop), usUsage=0x06 (Keyboard)
    rid[1].usUsagePage = 0x01;
    rid[1].usUsage = 0x06;
    rid[1].dwFlags = RIDEV_INPUTSINK;
    rid[1].hwndTarget = hwnd;

    if (!RegisterRawInputDevices(rid, 2, sizeof(rid[0]))) {
        // 注册失败可能是因为没有足够的权限或设备不可用
        OutputDebugStringA("Failed to register Raw Input devices.\n");
    }
}

// ==================== 窗口过程回调 ====================

/**
 * 处理 RAW INPUT 消息
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_INPUT) {
        UINT dwSize = 0;
        
        // 获取数据包大小
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
        
        if (dwSize > 0) {
            std::vector<BYTE> buffer(dwSize);
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer.data(), &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.data());

                // ==================== 处理鼠标数据 ====================
                if (raw->header.dwType == RIM_TYPEMOUSE) {
                    const RAWMOUSE& mouse = raw->data.mouse;
                    long dx = mouse.lLastX;
                    long dy = mouse.lLastY;
                    uintptr_t deviceHandle = reinterpret_cast<uintptr_t>(raw->header.hDevice);

                    // 累加位移（用于轮询模式）
                    g_accumulated_dx += dx;
                    g_accumulated_dy += dy;

                    // 触发移动回调（只有当有实际移动时）
                    if (g_mouse_callback && (dx != 0 || dy != 0)) {
                        py::gil_scoped_acquire acquire;
                        g_mouse_callback(deviceHandle, dx, dy);
                    }

                    // 检测鼠标按键事件
                    // 左键
                    if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
                        if (g_mouse_button_callback) {
                            py::gil_scoped_acquire acquire;
                            g_mouse_button_callback(deviceHandle, 1, true);
                        }
                    }
                    if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
                        if (g_mouse_button_callback) {
                            py::gil_scoped_acquire acquire;
                            g_mouse_button_callback(deviceHandle, 1, false);
                        }
                    }
                    // 右键
                    if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
                        if (g_mouse_button_callback) {
                            py::gil_scoped_acquire acquire;
                            g_mouse_button_callback(deviceHandle, 2, true);
                        }
                    }
                    if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
                        if (g_mouse_button_callback) {
                            py::gil_scoped_acquire acquire;
                            g_mouse_button_callback(deviceHandle, 2, false);
                        }
                    }
                    // 中键
                    if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
                        if (g_mouse_button_callback) {
                            py::gil_scoped_acquire acquire;
                            g_mouse_button_callback(deviceHandle, 3, true);
                        }
                    }
                    if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
                        if (g_mouse_button_callback) {
                            py::gil_scoped_acquire acquire;
                            g_mouse_button_callback(deviceHandle, 3, false);
                        }
                    }
                }
                // ==================== 处理键盘数据 ====================
                else if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                    uint16_t vkey = raw->data.keyboard.VKey;
                    bool is_down = !(raw->data.keyboard.Flags & RI_KEY_BREAK);

                    // 更新全局按键状态
                    {
                        std::lock_guard<std::mutex> lock(g_key_mutex);
                        if (is_down) {
                            g_pressed_keys.insert(vkey);
                        } else {
                            g_pressed_keys.erase(vkey);
                        }
                    }

                    // 触发 Python 回调
                    if (g_key_callback) {
                        py::gil_scoped_acquire acquire;
                        g_key_callback(reinterpret_cast<uintptr_t>(raw->header.hDevice), vkey, is_down);
                    }
                }
            }
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ==================== 消息循环线程 ====================

/**
 * 后台消息循环线程
 */
void MessageLoop() {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"HIDInputTrackerClass";
    RegisterClassW(&wc);

    // 创建隐藏的消息窗口（无可见UI）
    g_hwnd = CreateWindowExW(0, L"HIDInputTrackerClass", L"HIDInputTrackerWindow", 0, 
                             0, 0, 0, 0, HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    
    if (g_hwnd) {
        RegisterRawInput(g_hwnd);
        
        MSG msg;
        while (g_running && GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    if (g_hwnd) {
        UnregisterClassW(L"HIDInputTrackerClass", GetModuleHandle(nullptr));
    }
}

// ==================== Python API 函数 ====================

/**
 * 获取鼠标增量（并清零累加器）
 * @returns (dx, dy) 元组
 */
py::tuple get_mouse_delta() {
    long dx = g_accumulated_dx.exchange(0);
    long dy = g_accumulated_dy.exchange(0);
    return py::make_tuple(dx, dy);
}

/**
 * 获取当前所有按下的键
 * @returns 虚拟键码列表
 */
py::list get_pressed_keys() {
    std::lock_guard<std::mutex> lock(g_key_mutex);
    py::list keys;
    for (uint16_t key : g_pressed_keys) {
        keys.append(key);
    }
    return keys;
}

/**
 * 开始跟踪输入事件
 * @param mouse_cb 鼠标移动回调 (device_handle, dx, dy) -> None
 * @param key_cb 键盘回调 (device_handle, vkey, is_down) -> None  
 * @param mouse_button_cb 鼠标按键回调 (device_handle, button, is_down) -> None
 */
void start_tracking(py::object mouse_cb = py::none(), 
                    py::object key_cb = py::none(),
                    py::object mouse_button_cb = py::none()) {
    if (g_running) return;
    
    // 设置回调
    g_mouse_callback = mouse_cb.is_none() ? nullptr : mouse_cb.cast<std::function<void(uintptr_t, long, long)>>();
    g_key_callback = key_cb.is_none() ? nullptr : key_cb.cast<std::function<void(uintptr_t, uint16_t, bool)>>();
    g_mouse_button_callback = mouse_button_cb.is_none() ? nullptr : mouse_button_cb.cast<std::function<void(uintptr_t, uint32_t, bool)>>();

    g_running = true;
    {
        py::gil_scoped_release release;
        g_workerThread = std::thread(MessageLoop);
    }
}

/**
 * 停止跟踪输入事件
 */
void stop_tracking() {
    g_running = false;
    if (g_hwnd) {
        PostMessage(g_hwnd, WM_CLOSE, 0, 0);
    }
    if (g_workerThread.joinable()) {
        g_workerThread.join();
    }
    
    // 清理状态
    g_hwnd = nullptr;
    g_mouse_callback = nullptr;
    g_key_callback = nullptr;
    g_mouse_button_callback = nullptr;
    g_accumulated_dx = 0;
    g_accumulated_dy = 0;
    
    std::lock_guard<std::mutex> lock(g_key_mutex);
    g_pressed_keys.clear();
}

/**
 * 检查跟踪是否正在运行
 */
bool is_tracking() {
    return g_running.load();
}

// ==================== pybind11 模块定义 ====================

PYBIND11_MODULE(input_tracker, m) {
    m.doc() = "HIDController - Windows RAW INPUT keyboard and mouse event tracker";
    
    // 启动/停止跟踪
    m.def("start", &start_tracking, 
          py::arg("mouse_callback") = py::none(), 
          py::arg("key_callback") = py::none(),
          py::arg("mouse_button_callback") = py::none(), 
          "Start tracking keyboard and mouse events via RAW INPUT");
    
    m.def("stop", &stop_tracking, "Stop tracking");
    
    m.def("is_tracking", &is_tracking, "Check if tracking is running");
    
    // 轮询接口
    m.def("get_mouse_delta", &get_mouse_delta, "Get accumulated mouse delta since last call");
    m.def("get_pressed_keys", &get_pressed_keys, "Get list of currently pressed virtual key codes");
}
