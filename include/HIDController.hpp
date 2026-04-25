// DriveMouse.hpp - HIDController API
#pragma once
#include <stdint.h>
#include <vector>

// 基础Windows类型定义（不依赖windows.h）
// 注意：使用时需要包含windows.h，这里使用前向声明
struct HWND__;
typedef struct HWND__* HWND;

#ifndef WINAPI
#define WINAPI __stdcall
#endif
#ifdef DLL1_EXPORTS
#define DLLAPI  extern "C" __declspec(dllexport)
#else
#define DLLAPI  extern "C" __declspec(dllimport)
#endif

// -------------------- 鼠标控制 IbSendMouse.hpp --------------------

DLLAPI bool WINAPI MouseMoveRelative(int32_t dx, int32_t dy);
DLLAPI bool WINAPI MouseMoveAbsolute(uint32_t x, uint32_t y);
DLLAPI bool WINAPI MouseDown(uint16_t button);
DLLAPI bool WINAPI MouseUp(uint16_t button);
DLLAPI bool WINAPI MouseClick(uint16_t button);
DLLAPI bool WINAPI MouseWheel(int32_t movement);
DLLAPI void WINAPI SetMouseMoveCoefficient(float coefficient);
DLLAPI void WINAPI AutoCalibrate();
DLLAPI void WINAPI DisableMouseAcceleration();
DLLAPI void WINAPI EnableMouseAcceleration();

// -------------------- 键盘控制 IbSendKeyboard.hpp --------------------

DLLAPI bool WINAPI KeyDown(uint16_t vk);
DLLAPI bool WINAPI KeyUp(uint16_t vk);
DLLAPI bool WINAPI KeyPress(uint16_t vk);
DLLAPI bool WINAPI KeyCombo(const std::vector<uint16_t>& keys);
DLLAPI bool WINAPI KeySeq(const std::vector<uint16_t>& keys);
DLLAPI void WINAPI release_all_keys();

// -------------------- Raw Input 设备检测功能 --------------------

typedef void (*MouseMoveCallback)(uintptr_t deviceHandle, int32_t dx, int32_t dy);
typedef void (*MouseButtonCallback)(uintptr_t deviceHandle, uint32_t button, bool isDown);
typedef void (*KeyboardCallback)(uintptr_t deviceHandle, uint16_t vkey, bool isDown);

DLLAPI bool WINAPI StartInputTracking(MouseMoveCallback mouse_cb, KeyboardCallback key_cb, MouseButtonCallback mouse_button_cb);
DLLAPI void WINAPI StopInputTracking();
DLLAPI bool WINAPI IsTracking();
DLLAPI void WINAPI GetMouseDelta(int32_t* dx, int32_t* dy);
DLLAPI void WINAPI GetPressedKeys(uint16_t* keys, uint32_t* count);
DLLAPI bool WINAPI HIDController_RegisterRawInput(HWND hwnd);
