//IbSendMouse.hpp
#pragma once
#include "SendTypes.hpp"

#ifdef __cplusplus
extern "C" {
#endif

	// 低级鼠标事件
	VOID WINAPI IbSend_mouse_event(
		_In_ DWORD dwFlags,
		_In_ DWORD dx,
		_In_ DWORD dy,
		_In_ DWORD dwData,
		_In_ ULONG_PTR dwExtraInfo
	);

	// 移动鼠标
	bool WINAPI IbSendMouseMove(uint32_t x, uint32_t y, Send::MoveMode mode);

	// 鼠标点击
	bool WINAPI IbSendMouseClick(Send::MouseButton button);

	// 鼠标滚轮
	bool WINAPI IbSendMouseWheel(int32_t movement);

#ifdef __cplusplus
}
#endif
