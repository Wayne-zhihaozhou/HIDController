#pragma once
#include "dll_export.hpp"
#include <SendTypes.hpp>
#include "SendInputHook.hpp"
using namespace Send;

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
