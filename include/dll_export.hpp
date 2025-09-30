#pragma once
#include "SendTypes.hpp"

#ifdef DLL1_EXPORTS
#define DLLAPI extern "C" __declspec(dllexport)
#else
#define DLLAPI extern "C" __declspec(dllimport)
#endif


// 初始化发送模块
DLLAPI Send::Error __stdcall IbSendInit(Send::SendType type, Send::InitFlags flags, void* argument);

// 销毁发送模块
DLLAPI void __stdcall IbSendDestroy();

// 同步键盘状态
DLLAPI void __stdcall IbSendSyncKeyStates();

// 
DLLAPI VOID WINAPI IbSend_mouse_event(
    _In_ DWORD dwFlags,
    _In_ DWORD dx,
    _In_ DWORD dy,
    _In_ DWORD dwData,
    _In_ ULONG_PTR dwExtraInfo
);

// ======================
// C++ 接口封装函数
// ======================

namespace Send {

    // 移动鼠标
    DLLAPI bool __stdcall IbSendMouseMove(uint32_t x, uint32_t y, Send::MoveMode mode);

    // 鼠标点击
    DLLAPI bool __stdcall IbSendMouseClick(Send::MouseButton button);

    // 鼠标滚轮
    DLLAPI bool __stdcall IbSendMouseWheel(int32_t movement);
}

