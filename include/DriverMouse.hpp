//DriveMouse.hpp
#pragma once
#include "SendTypes.hpp"



// -------------------- 初始化/销毁 IbSendBase.hpp --------------------

// 初始化发送模块
DLLAPI Send::Error WINAPI IbSendInit(Send::SendType type, Send::InitFlags flags, void* argument);

// 销毁发送模块
DLLAPI void WINAPI IbSendDestroy();

// 同步键盘状态
DLLAPI void WINAPI IbSendSyncKeyStates();

// -------------------- 鼠标控制 IbSendMouse.hpp --------------------

// 移动鼠标
DLLAPI bool WINAPI MouseMove(uint32_t x, uint32_t y, Send::MoveMode mode);

// 鼠标点击
DLLAPI bool WINAPI MouseClick(Send::MouseButton button);

// 鼠标滚轮
DLLAPI bool WINAPI MouseWheel(int32_t movement);

// -------------------- 键盘控制 IbSendKeyboard.hpp --------------------
// 按键
DLLAPI bool WINAPI KeyDown(uint16_t vk);
DLLAPI bool WINAPI KeyUp(uint16_t vk);
DLLAPI bool WINAPI KeyPress(uint16_t vk);
DLLAPI bool WINAPI KeyCombo(uint16_t vk, Send::KeyboardModifiers modifiers);







