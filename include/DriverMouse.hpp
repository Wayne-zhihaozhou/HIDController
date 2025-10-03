//DriveMouse.hpp
#pragma once
#include <vector>
#include <cstdint>


#ifdef DLL1_EXPORTS
#define DLLAPI  extern "C" __declspec(dllexport)
#else
#define DLLAPI  extern "C" __declspec(dllimport)
#endif

// -------------------- 初始化/销毁 IbSendBase.hpp --------------------


// -------------------- 鼠标控制 IbSendMouse.hpp --------------------
DLLAPI bool WINAPI MouseMoveRelative(int32_t dx, int32_t dy);
DLLAPI bool WINAPI MouseMoveAbsolute(uint32_t x, uint32_t y);
DLLAPI bool WINAPI MouseDown(uint16_t button);
DLLAPI bool WINAPI MouseUp(uint16_t button);
DLLAPI bool WINAPI MouseClick(uint16_t button);
DLLAPI bool WINAPI MouseWheel(int32_t movement);



// -------------------- 键盘控制 IbSendKeyboard.hpp --------------------
DLLAPI bool WINAPI KeyDown(uint16_t vk);
DLLAPI bool WINAPI KeyUp(uint16_t vk);
DLLAPI bool WINAPI KeyPress(uint16_t vk);
DLLAPI bool WINAPI KeyCombo(const std::vector<uint16_t>& keys);






