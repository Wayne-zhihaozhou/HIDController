//DriveMouse.hpp
#pragma once
#include "SendTypes.hpp"
#include <vector>
#include <cstdint>
#include <Logitech.hpp>


// -------------------- 初始化/销毁 IbSendBase.hpp --------------------

// 初始化发送模块
DLLAPI bool WINAPI IbSendInit();

// 销毁发送模块
DLLAPI void WINAPI IbSendDestroy();


// -------------------- 鼠标控制 IbSendMouse.hpp --------------------
DLLAPI bool WINAPI MouseMoveRelative(int32_t dx, int32_t dy);
DLLAPI bool WINAPI MouseMoveAbsolute(uint32_t x, uint32_t y);
DLLAPI bool WINAPI MouseDown(Send::Internal::Logitech::MouseButton button);
DLLAPI bool WINAPI MouseUp(Send::Internal::Logitech::MouseButton button);
DLLAPI bool WINAPI MouseClick(Send::Internal::Logitech::MouseButton button);
DLLAPI bool WINAPI MouseWheel(int32_t movement);



// -------------------- 键盘控制 IbSendKeyboard.hpp --------------------
DLLAPI bool WINAPI KeyDown(uint16_t vk);
DLLAPI bool WINAPI KeyUp(uint16_t vk);
DLLAPI bool WINAPI KeyPress(uint16_t vk);
DLLAPI bool WINAPI KeyCombo(const std::vector<uint16_t>& keys);






