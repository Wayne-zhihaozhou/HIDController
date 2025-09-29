#pragma once
#include <Windows.h>
#include "dll_export.hpp"
#include "SendTypes.hpp"

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
    DLLAPI void __stdcall IbSendInputHook(Send::HookCode code);

#ifdef __cplusplus
}
#endif


DLLAPI void __stdcall IbSendInputHook(HookCode code) {
    switch (code) {
    case HookCode::InitOnly:
        sendinput_hook.create(); // 初始化钩子
        break;
    case HookCode::Destroy:
        sendinput_hook.destroy(); // 卸载钩子
        break;
    case HookCode::On:
        if (!sendinput_hook.created())
            sendinput_hook.create();
        sendinput_hook->hook = true; // 启用钩子
        break;
    case HookCode::Off:
        sendinput_hook->hook = false; // 禁用钩子
        break;
    }
}

