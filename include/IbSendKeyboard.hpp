//IbSendKeyboard.hpp
#pragma once
#include <Windows.h>
#include "dll_export.hpp"
#include "SendTypes.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    // 低级键盘事件
    DLLAPI VOID WINAPI IbSend_keybd_event(
        _In_ BYTE bVk,
        _In_ BYTE bScan,
        _In_ DWORD dwFlags,
        _In_ ULONG_PTR dwExtraInfo
    );

#ifdef __cplusplus
}
#endif

// 高级封装键盘操作
#ifdef __cplusplus
namespace Send {

    // 按下指定键
    DLLAPI bool __stdcall IbSendKeybdDown(uint16_t vk);

    // 释放指定键
    DLLAPI bool __stdcall IbSendKeybdUp(uint16_t vk);

    // 按下并释放指定键，可附加修饰键
    DLLAPI bool __stdcall IbSendKeybdDownUp(uint16_t vk, KeyboardModifiers modifiers);

}  // namespace Send
#endif
