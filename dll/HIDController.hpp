//DriveMouse.hpp
#pragma once
#include <stdint.h>
#include <initializer_list>
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#ifdef DLL1_EXPORTS
#define DLLAPI  extern "C" __declspec(dllexport)
#else
#define DLLAPI  extern "C" __declspec(dllimport)
#endif

// -------------------- ������ IbSendMouse.hpp --------------------

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

// -------------------- ���̿��� IbSendKeyboard.hpp --------------------

DLLAPI bool WINAPI KeyDown(uint16_t vk);
DLLAPI bool WINAPI KeyUp(uint16_t vk);
DLLAPI bool WINAPI KeyPress(uint16_t vk);
DLLAPI bool WINAPI KeyCombo(const std::initializer_list<uint16_t>& keys);
DLLAPI bool WINAPI KeySeq(const std::initializer_list<uint16_t>& keys);






