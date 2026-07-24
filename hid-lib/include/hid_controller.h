// hid_controller.h - HIDController API
#pragma once
#include <stdint.h>
#include <vector>
#include <utility>

#include "mouse_button.h"
#include "virtual_keys.h"

// 基础Windows类型定义（不依赖windows.h）
// 注意：使用时需要包含windows.h，这里使用前向声明
struct HWND__;
typedef struct HWND__* HWND;

#ifndef WINAPI
#define WINAPI __stdcall
#endif
#ifdef hid_controller_EXPORTS
#define DLLAPI  extern "C" __declspec(dllexport)
#define DLLAPI_CPP __declspec(dllexport)
#else
#define DLLAPI  extern "C" __declspec(dllimport)
#define DLLAPI_CPP __declspec(dllimport)
#endif

// -------------------- 鼠标控制 --------------------

DLLAPI bool WINAPI mouse_move_relative(int32_t dx, int32_t dy);
DLLAPI bool WINAPI mouse_move_absolute(uint32_t x, uint32_t y);
DLLAPI bool WINAPI mouse_down(MouseButton button);
DLLAPI bool WINAPI mouse_up(MouseButton button);
DLLAPI bool WINAPI mouse_click(MouseButton button);
DLLAPI bool WINAPI mouse_wheel(int32_t movement);
DLLAPI void WINAPI set_mouse_move_coefficient(float coefficient);
DLLAPI float WINAPI get_mouse_move_coefficient();
DLLAPI void WINAPI auto_calibrate();
DLLAPI void WINAPI disable_mouse_acceleration();
DLLAPI void WINAPI enable_mouse_acceleration();

// -------------------- 键盘控制 --------------------

DLLAPI bool WINAPI key_down(KeyCode vk);
DLLAPI bool WINAPI key_up(KeyCode vk);
DLLAPI bool WINAPI key_press(KeyCode vk);
DLLAPI bool WINAPI key_combo(const std::vector<KeyCode>& keys);
DLLAPI bool WINAPI key_seq(const std::vector<KeyCode>& keys);
DLLAPI void WINAPI release_all_keys();

// -------------------- 临时按键拦截+排队重放 --------------------

DLLAPI void WINAPI begin_key_intercept();
DLLAPI void WINAPI end_key_intercept();
DLLAPI void WINAPI discard_queued_keys();

// -------------------- 临时鼠标拦截+排队重放 --------------------

DLLAPI void WINAPI begin_mouse_intercept();
DLLAPI void WINAPI end_mouse_intercept();
DLLAPI void WINAPI discard_queued_mouse_events();

// -------------------- Raw Input 设备检测功能 --------------------

typedef void (*mouse_move_callback)(uintptr_t device_handle, int32_t dx, int32_t dy);
typedef void (*mouse_button_callback)(uintptr_t device_handle, uint32_t button, bool is_down);
typedef void (*keyboard_callback)(uintptr_t device_handle, uint16_t vkey, bool is_down);
typedef void (*mouse_wheel_callback)(uintptr_t device_handle, int32_t wheel_delta, int32_t horizontal);

DLLAPI bool WINAPI start_input_tracking(mouse_move_callback mouse_callback, keyboard_callback key_callback, mouse_button_callback mouse_button_callback, mouse_wheel_callback wheel_callback);
DLLAPI void WINAPI stop_input_tracking();
DLLAPI bool WINAPI is_tracking();
DLLAPI bool WINAPI register_raw_input(HWND hwnd);
DLLAPI bool WINAPI get_device_name(uintptr_t device_handle, wchar_t* name, uint32_t* name_length);
// Raw input query — uses C++ linkage for std::pair/std::vector return types
DLLAPI_CPP std::pair<long, long> get_mouse_delta();
DLLAPI_CPP std::pair<long, long> get_mouse_delta(uintptr_t device_handle);
DLLAPI_CPP std::vector<uint16_t> get_pressed_keys();
