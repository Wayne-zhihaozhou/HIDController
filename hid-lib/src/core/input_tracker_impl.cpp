// input_tracker_impl.cpp — Pure C++ input tracking implementation (no pybind11)
// HIDController project — Windows RAW INPUT API keyboard and mouse event detection
#include "input_tracker_internal.h"

// ==================== Global state ====================
static std::atomic<bool> running_{false};
static std::thread worker_thread_;
static HWND hwnd_ = nullptr;

static mouse_move_fn mouse_callback_;
static keyboard_fn key_callback_;
static mouse_button_fn mouse_button_callback_;
static mouse_wheel_fn mouse_wheel_callback_;

static std::atomic<long> accumulated_dx_{0};
static std::atomic<long> accumulated_dy_{0};

static std::mutex key_mutex_;
static std::set<uint16_t> pressed_keys_;

// ==================== RAW Input Registration ====================

static void register_raw_input(HWND hwnd) {
	RAWINPUTDEVICE rid[2];

	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x02;
	rid[0].dwFlags = RIDEV_INPUTSINK;
	rid[0].hwndTarget = hwnd;

	rid[1].usUsagePage = 0x01;
	rid[1].usUsage = 0x06;
	rid[1].dwFlags = RIDEV_INPUTSINK;
	rid[1].hwndTarget = hwnd;

	if (!RegisterRawInputDevices(rid, 2, sizeof(rid[0]))) {
		OutputDebugStringA("Failed to register Raw Input devices.\n");
	}
}

// ==================== Device Name Query ====================

std::wstring get_device_name_impl(uintptr_t device_handle) {
    HANDLE device = reinterpret_cast<HANDLE>(device_handle);
    if (device == INVALID_HANDLE_VALUE || device == nullptr)
        return L"";

    // First get the required buffer size
    DWORD buffer_size = 0;
    if (!GetRawInputDeviceInfoW(device, RIDI_DEVICENAME, nullptr, &buffer_size))
        return L"";

    // buffer_size includes null terminator
    if (buffer_size == 0 || buffer_size > 1024)
        return L"";

    std::wstring name(buffer_size, L'\0');
    if (GetRawInputDeviceInfoW(device, RIDI_DEVICENAME, &name.front(), &buffer_size) == 0)
        return L"";

    // Strip trailing nulls
    while (!name.empty() && name.back() == L'\0')
        name.pop_back();
    return name;
}

// ==================== Window Procedure ====================

static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_INPUT) {
		UINT size = 0;
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

		if (size > 0) {
			std::vector<BYTE> buffer(size);
			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER)) == size) {
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.data());

				if (raw->header.dwType == RIM_TYPEMOUSE) {
					const RAWMOUSE& mouse = raw->data.mouse;
					long dx = mouse.lLastX;
					long dy = mouse.lLastY;
					uintptr_t device_handle = reinterpret_cast<uintptr_t>(raw->header.hDevice);

					accumulated_dx_ += dx;
					accumulated_dy_ += dy;

					if (mouse_callback_ && (dx != 0 || dy != 0)) {
						mouse_callback_(device_handle, dx, dy);
					}

					#define CHECK_BTN(flag, num) \
						if (mouse.usButtonFlags & flag) { \
							if (mouse_button_callback_) mouse_button_callback_(device_handle, num, mouse.usButtonFlags & flag); \
						}
					CHECK_BTN(RI_MOUSE_LEFT_BUTTON_DOWN, 1)
					CHECK_BTN(RI_MOUSE_LEFT_BUTTON_UP, 1)
					CHECK_BTN(RI_MOUSE_RIGHT_BUTTON_DOWN, 2)
					CHECK_BTN(RI_MOUSE_RIGHT_BUTTON_UP, 2)
					CHECK_BTN(RI_MOUSE_MIDDLE_BUTTON_DOWN, 3)
					CHECK_BTN(RI_MOUSE_MIDDLE_BUTTON_UP, 3)

					// XButton1/XButton2
					if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) {
						if (mouse_button_callback_) mouse_button_callback_(device_handle, 4, true);
					}
					if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) {
						if (mouse_button_callback_) mouse_button_callback_(device_handle, 4, false);
					}
					if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) {
						if (mouse_button_callback_) mouse_button_callback_(device_handle, 5, true);
					}
					if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) {
						if (mouse_button_callback_) mouse_button_callback_(device_handle, 5, false);
					}

					// Vertical wheel
					if (mouse.usButtonFlags & RI_MOUSE_WHEEL) {
						if (mouse_wheel_callback_) {
							WORD raw_wheel = static_cast<WORD>(mouse.usButtonData);
							int32_t wheel_delta = (raw_wheel > 32767) ? (static_cast<int32_t>(raw_wheel) - 65536) : static_cast<int32_t>(raw_wheel);
							mouse_wheel_callback_(device_handle, wheel_delta, false);
						}
					}

					// Horizontal wheel (Windows 8+)
					if (mouse.usButtonFlags & RI_MOUSE_HWHEEL) {
						if (mouse_wheel_callback_) {
							WORD raw_hwheel = static_cast<WORD>(mouse.usButtonData);
							int32_t hwheel_delta = (raw_hwheel > 32767) ? (static_cast<int32_t>(raw_hwheel) - 65536) : static_cast<int32_t>(raw_hwheel);
							mouse_wheel_callback_(device_handle, hwheel_delta, true);
						}
					}
				}
				else if (raw->header.dwType == RIM_TYPEKEYBOARD) {
					uint16_t vkey = raw->data.keyboard.VKey;
					bool is_down = !(raw->data.keyboard.Flags & RI_KEY_BREAK);

					{
						std::lock_guard<std::mutex> lock(key_mutex_);
						if (is_down) pressed_keys_.insert(vkey);
						else pressed_keys_.erase(vkey);
					}

					if (key_callback_) {
						key_callback_(reinterpret_cast<uintptr_t>(raw->header.hDevice), vkey, is_down);
					}
				}
			}
		}
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ==================== Message Loop ====================

static void message_loop() {
	WNDCLASSW wc = {};
	wc.lpfnWndProc = wnd_proc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = L"HIDInputTrackerClass";
	RegisterClassW(&wc);

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

// ==================== Public API ====================

void start_tracking_impl(mouse_move_fn mc, keyboard_fn kc, mouse_button_fn mbc, mouse_wheel_fn mwc) {
	if (running_) return;
	mouse_callback_ = mc;
	key_callback_ = kc;
	mouse_button_callback_ = mbc;
	mouse_wheel_callback_ = mwc;
	running_ = true;
	worker_thread_ = std::thread(message_loop);
}

void stop_tracking_impl() {
	running_ = false;
	if (hwnd_) PostMessage(hwnd_, WM_CLOSE, 0, 0);
	if (worker_thread_.joinable()) worker_thread_.join();

	hwnd_ = nullptr;
	mouse_callback_ = nullptr;
	key_callback_ = nullptr;
	mouse_button_callback_ = nullptr;
	mouse_wheel_callback_ = nullptr;
	accumulated_dx_ = 0;
	accumulated_dy_ = 0;

	std::lock_guard<std::mutex> lock(key_mutex_);
	pressed_keys_.clear();
}

bool is_tracking_impl() {
	return running_.load();
}

std::pair<long, long> get_mouse_delta_impl() {
	long dx = accumulated_dx_.exchange(0);
	long dy = accumulated_dy_.exchange(0);
	return {dx, dy};
}

std::vector<uint16_t> get_pressed_keys_impl() {
	std::lock_guard<std::mutex> lock(key_mutex_);
	std::vector<uint16_t> keys(pressed_keys_.begin(), pressed_keys_.end());
	return keys;
}
