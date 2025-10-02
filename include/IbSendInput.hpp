//IbSendInput.hpp
#pragma once
#include <Windows.h>
#include "dll_export.hpp"
#include "SendTypes.hpp"
#include "SendInputHook.hpp"




#ifdef __cplusplus
extern "C" {
#endif

    // 发送输入事件
    DLLAPI UINT WINAPI IbSendInput(
        _In_ UINT cInputs,
        _In_reads_(cInputs) LPINPUT pInputs,
        _In_ int cbSize
    );

    // 安装或触发输入钩子
    DLLAPI void WINAPI IbSendInputHook(Send::HookCode code);

#ifdef __cplusplus
}
#endif




