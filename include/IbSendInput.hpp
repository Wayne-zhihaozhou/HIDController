#pragma once
#include <Windows.h>
#include "dll_export.hpp"
#include "SendTypes.hpp"
#include "SendInputHook.hpp"

//using namespace Send;


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


//重写
DLLAPI void __stdcall IbSendInputHook(Send::HookCode code) {
    switch (code) {
    case Send::HookCode::InitOnly:
        if (!g_sendInputHook)
            g_sendInputHook = std::make_unique<SendInputHook>();
        break;

    case Send::HookCode::Destroy:
        g_sendInputHook.reset();  // 自动销毁 SendInputHook
        break;

    case Send::HookCode::On:
        if (!g_sendInputHook)
            g_sendInputHook = std::make_unique<SendInputHook>();
        SendInputHook::hook = true; // 启用钩子
        break;

    case Send::HookCode::Off:
        SendInputHook::hook = false; // 禁用钩子
        break;
    }
}


//DLLAPI void __stdcall IbSendInputHook(Send::HookCode code) {
//    switch (code) {
//    case Send::HookCode::InitOnly:
//        sendinput_hook.create(); // 初始化钩子
//        break;
//    case Send::HookCode::Destroy:
//        sendinput_hook.destroy(); // 卸载钩子
//        break;
//    case Send::HookCode::On:
//        if (!sendinput_hook.created())
//            sendinput_hook.create();
//        sendinput_hook->hook = true; // 启用钩子
//        break;
//    case Send::HookCode::Off:
//        sendinput_hook->hook = false; // 禁用钩子
//        break;
//    }
//}

