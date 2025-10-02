//DriveMouse.hpp
#pragma once
#include <SendTypes.hpp>


//-------------------- 定义DLL导出宏 ----------
#ifdef DLL1_EXPORTS
#define DLLAPI extern "C" __declspec(dllexport)//C语言链接方式;函数导出到DLL中
#else
#define DLLAPI extern "C" __declspec(dllimport)//C语言链接方式;函数导入到DLL中
#endif


//使用C风格
#ifdef __cplusplus
extern "C" {
#endif

	// -------------------- 初始化/销毁 --------------------
	
	// 初始化发送模块
	DLLAPI Send::Error WINAPI IbSendInit(Send::SendType type, Send::InitFlags flags, void* argument);

	// 销毁发送模块
	DLLAPI void WINAPI IbSendDestroy();

	// 同步键盘状态
	DLLAPI void WINAPI IbSendSyncKeyStates();

	// -------------------- 鼠标控制 --------------------

	// 移动鼠标
	DLLAPI bool WINAPI IbSendMouseMove(uint32_t x, uint32_t y, Send::MoveMode mode);

	// 鼠标点击
	DLLAPI bool WINAPI IbSendMouseClick(Send::MouseButton button);

	// 鼠标滚轮
	DLLAPI bool WINAPI IbSendMouseWheel(int32_t movement);

	// -------------------- 键盘控制 --------------------
	struct KeyboardModifiers;

	// 按下指定键
	DLLAPI bool WINAPI IbSendKeybdDown(uint16_t vk);

	// 释放指定键
	DLLAPI bool WINAPI IbSendKeybdUp(uint16_t vk);

	// 按下并释放指定键，可附加修饰键
	DLLAPI bool WINAPI IbSendKeybdDownUp(uint16_t vk, KeyboardModifiers modifiers);




#ifdef __cplusplus
}
#endif





