// input_tracker.cpp - Windows RAW INPUT API keyboard and mouse event detection
// HIDController project
//
// This file provides the C++ implementation. It should be #include'd by
// src/bindings/input_tracker_bindings.cpp which defines the PYBIND11_MODULE.
//
// Note: This file uses pybind11 types (py::tuple, py::list, py::object, py::gil_scoped_acquire).
// The #include "input_tracker.cpp" pattern is used in input_tracker_bindings.cpp,
// so the pybind11 headers must be included in the bindings file first.

#include <windows.h>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <set>
#include <functional>

// ==================== Global variables ====================
std::atomic<bool> running_(false);
std::thread worker_thread_;
HWND hwnd_ = nullptr;

// Callback function handles
std::function<void(uintptr_t, long, long)> mouse_callback_ = nullptr;
std::function<void(uintptr_t, uint16_t, bool)> key_callback_ = nullptr;
std::function<void(uintptr_t, uint32_t, bool)> mouse_button_callback_ = nullptr;

// Mouse delta accumulators
std::atomic<long> accumulated_dx_{0};
std::atomic<long> accumulated_dy_{0};

// Keyboard state tracking
std::mutex key_mutex_;
std::set<uint16_t> pressed_keys_; // Tracks currently pressed virtual key codes (VKEY)

// ==================== RAW Input Registration ====================

/**
 * Register Raw Input to listen for mouse and keyboard events
 */
void register_raw_input(HWND hwnd) {
	RAWINPUTDEVICE rid[2];

	// Mouse - usUsagePage=0x01 (Generic Desktop), usUsage=0x02 (Mouse)
	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x02;
	rid[0].dwFlags = RIDEV_INPUTSINK; // Receive even when window loses focus
	rid[0].hwndTarget = hwnd;

	// Keyboard - usUsagePage=0x01 (Generic Desktop), usUsage=0x06 (Keyboard)
	rid[1].usUsagePage = 0x01;
	rid[1].usUsage = 0x06;
	rid[1].dwFlags = RIDEV_INPUTSINK;
	rid[1].hwndTarget = hwnd;

	if (!RegisterRawInputDevices(rid, 2, sizeof(rid[0]))) {
		OutputDebugStringA("Failed to register Raw Input devices.\n");
	}
}

// ==================== Window Procedure ====================

/**
 * Handle RAW INPUT messages
 */
LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_INPUT) {
		UINT size = 0;

		// Get data packet size
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

		if (size > 0) {
			std::vector<BYTE> buffer(size);
			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER)) == size) {
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.data());

				// ==================== Process mouse data ====================
				if (raw->header.dwType == RIM_TYPEMOUSE) {
					const RAWMOUSE& mouse = raw->data.mouse;
					long dx = mouse.lLastX;
					long dy = mouse.lLastY;
					uintptr_t device_handle = reinterpret_cast<uintptr_t>(raw->header.hDevice);

					// Accumulate delta (for polling mode)
					accumulated_dx_ += dx;
					accumulated_dy_ += dy;

					// Trigger mouse move callback (only on actual movement)
					if (mouse_callback_ && (dx != 0 || dy != 0)) {
						py::gil_scoped_acquire acquire;
						mouse_callback_(device_handle, dx, dy);
					}

					// Detect mouse button events
					if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
						if (mouse_button_callback_) {
							py::gil_scoped_acquire acquire;
							mouse_button_callback_(device_handle, 1, true);
						}
					}
					if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
						if (mouse_button_callback_) {
							py::gil_scoped_acquire acquire;
							mouse_button_callback_(device_handle, 1, false);
						}
					}
					if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
						if (mouse_button_callback_) {
							py::gil_scoped_acquire acquire;
							mouse_button_callback_(device_handle, 2, true);
						}
					}
					if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
						if (mouse_button_callback_) {
							py::gil_scoped_acquire acquire;
							mouse_button_callback_(device_handle, 2, false);
						}
					}
					if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
						if (mouse_button_callback_) {
							py::gil_scoped_acquire acquire;
							mouse_button_callback_(device_handle, 3, true);
						}
					}
					if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
						if (mouse_button_callback_) {
							py::gil_scoped_acquire acquire;
							mouse_button_callback_(device_handle, 3, false);
						}
					}
				}
				// ==================== Process keyboard data ====================
				else if (raw->header.dwType == RIM_TYPEKEYBOARD) {
					uint16_t vkey = raw->data.keyboard.VKey;
					bool is_down = !(raw->data.keyboard.Flags & RI_KEY_BREAK);

					// Update global key state
					{
						std::lock_guard<std::mutex> lock(key_mutex_);
						if (is_down) {
							pressed_keys_.insert(vkey);
						} else {
							pressed_keys_.erase(vkey);
						}
					}

					// Trigger Python callback
					if (key_callback_) {
						py::gil_scoped_acquire acquire;
						key_callback_(reinterpret_cast<uintptr_t>(raw->header.hDevice), vkey, is_down);
					}
				}
			}
		}
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ==================== Message Loop Thread ====================

/**
 * Background message loop thread
 */
void message_loop() {
	WNDCLASSW wc = {};
	wc.lpfnWndProc = wnd_proc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = L"HIDInputTrackerClass";
	RegisterClassW(&wc);

	// Create hidden message window (no visible UI)
	hwnd_ = CreateWindowExW(0, L"HIDInputTrackerClass", L"HIDInputTrackerWindow", 0,
							0, 0, 0, 0, HWND_MESSAGE, nullptr, wc.hInstance, nullptr);

	if (hwnd_) {
		register_raw_input(hwnd_);

		MSG msg;
		while (running_ && GetMessage(&msg, nullptr, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	if (hwnd_) {
		UnregisterClassW(L"HIDInputTrackerClass", GetModuleHandle(nullptr));
	}
}

// ==================== Python API Functions ====================

/**
 * Get accumulated mouse delta and reset accumulators.
 * @returns (dx, dy) tuple
 */
py::tuple get_mouse_delta() {
	long dx = accumulated_dx_.exchange(0);
	long dy = accumulated_dy_.exchange(0);
	return py::make_tuple(dx, dy);
}

/**
 * Get all currently pressed keys.
 * @returns list of virtual key codes
 */
py::list get_pressed_keys() {
	std::lock_guard<std::mutex> lock(key_mutex_);
	py::list keys;
	for (uint16_t key : pressed_keys_) {
		keys.append(key);
	}
	return keys;
}

/**
 * Start tracking input events.
 * @param mouse_cb Mouse move callback (device_handle, dx, dy) -> None
 * @param key_cb Keyboard callback (device_handle, vkey, is_down) -> None
 * @param mouse_button_cb Mouse button callback (device_handle, button, is_down) -> None
 */
void start_tracking(py::object mouse_cb = py::none(),
					py::object key_cb = py::none(),
					py::object mouse_button_cb = py::none()) {
	if (running_) return;

	// Set callbacks
	mouse_callback_ = mouse_cb.is_none() ? nullptr : mouse_cb.cast<std::function<void(uintptr_t, long, long)>>();
	key_callback_ = key_cb.is_none() ? nullptr : key_cb.cast<std::function<void(uintptr_t, uint16_t, bool)>>();
	mouse_button_callback_ = mouse_button_cb.is_none() ? nullptr : mouse_button_cb.cast<std::function<void(uintptr_t, uint32_t, bool)>>();

	running_ = true;
	{
		py::gil_scoped_release release;
		worker_thread_ = std::thread(message_loop);
	}
}

/**
 * Stop tracking input events.
 */
void stop_tracking() {
	running_ = false;
	if (hwnd_) {
		PostMessage(hwnd_, WM_CLOSE, 0, 0);
	}
	if (worker_thread_.joinable()) {
		worker_thread_.join();
	}

	// Cleanup state
	hwnd_ = nullptr;
	mouse_callback_ = nullptr;
	key_callback_ = nullptr;
	mouse_button_callback_ = nullptr;
	accumulated_dx_ = 0;
	accumulated_dy_ = 0;

	std::lock_guard<std::mutex> lock(key_mutex_);
	pressed_keys_.clear();
}

/**
 * Check if tracking is running.
 */
bool is_tracking() {
	return running_.load();
}
