#pragma once
#include <Windows.h>
#include <stdint.h>
#include "SendTypes.hpp"
#include "dll_export.hpp"

//// 初始化发送模块
//DLLAPI Send::Error __stdcall IbSendInit(Send::SendType type, Send::InitFlags flags, void* argument);
//
//// 销毁发送模块
//DLLAPI void __stdcall IbSendDestroy();
//
//// 同步键盘状态
//DLLAPI void __stdcall IbSendSyncKeyStates();

#pragma region API 1

//// 发送输入事件
//DLLAPI UINT WINAPI IbSendInput(
//    _In_ UINT cInputs,                    // number of input in the array
//    _In_reads_(cInputs) LPINPUT pInputs,  // array of inputs
//    _In_ int cbSize                       // sizeof(INPUT)
//);
//
//// 安装或触发输入钩子
//DLLAPI void __stdcall IbSendInputHook(Send::HookCode code);

#pragma endregion

#pragma region API 2

//// 模拟鼠标事件
//DLLAPI VOID WINAPI IbSend_mouse_event(
//    _In_ DWORD dwFlags,         //MOUSEEVENTF_
//    _In_ DWORD dx,
//    _In_ DWORD dy,
//    _In_ DWORD dwData,
//    _In_ ULONG_PTR dwExtraInfo
//);

//// 模拟键盘事件
//DLLAPI VOID WINAPI IbSend_keybd_event(
//    _In_ BYTE bVk,
//    _In_ BYTE bScan,
//    _In_ DWORD dwFlags,         //KEYEVENTF_
//    _In_ ULONG_PTR dwExtraInfo
//);

#pragma endregion

#pragma region API 3

//// 移动鼠标到指定位置
//DLLAPI bool __stdcall IbSendMouseMove(uint32_t x, uint32_t y, Send::MoveMode mode);
//
//// 鼠标点击指定按键
//DLLAPI bool __stdcall IbSendMouseClick(Send::MouseButton button);
//
//// 模拟鼠标滚轮滚动
//DLLAPI bool __stdcall IbSendMouseWheel(int32_t movement);

//// 按下指定键盘按键
//DLLAPI bool __stdcall IbSendKeybdDown(uint16_t vk);
//
//// 释放指定键盘按键
//DLLAPI bool __stdcall IbSendKeybdUp(uint16_t vk);
//
//// 按下并释放指定按键，同时可附加修饰键
//DLLAPI bool __stdcall IbSendKeybdDownUp(uint16_t vk, Send::KeyboardModifiers modifiers);

#pragma endregion
