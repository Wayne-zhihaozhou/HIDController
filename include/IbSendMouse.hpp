#pragma once
#include <Windows.h>
#include "dll_export.hpp"
#include "SendTypes.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    // 低级鼠标事件
    DLLAPI VOID WINAPI IbSend_mouse_event(
        _In_ DWORD dwFlags,
        _In_ DWORD dx,
        _In_ DWORD dy,
        _In_ DWORD dwData,
        _In_ ULONG_PTR dwExtraInfo
    );

#ifdef __cplusplus
}
#endif

// 高级封装鼠标操作
#ifdef __cplusplus
namespace Send {

    // 移动鼠标
    DLLAPI bool __stdcall IbSendMouseMove(uint32_t x, uint32_t y, MoveMode mode);

    // 鼠标点击
    DLLAPI bool __stdcall IbSendMouseClick(MouseButton button);

    // 鼠标滚轮
    DLLAPI bool __stdcall IbSendMouseWheel(int32_t movement);

}  // namespace Send
#endif
