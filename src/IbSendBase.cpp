//ibSendBase.cpp
#include"pch.h"
#include "Logitech.hpp"
#include "SendInputHook.hpp"

// 初始化发送模块（仅支持 Logitech），创建并保存全局发送对象
DLLAPI bool WINAPI IbSendInit() {

    auto logitech = std::make_unique<Send::Internal::Logitech>();//待修改,创建全局变量
    if (!logitech->create())
        return false;

    Send::g_send = std::move(logitech); 
    return true;
}

// 销毁发送模块，释放全局发送对象
DLLAPI void WINAPI IbSendDestroy() {

    if (!Send::g_send)
        return;
    Send::g_send->destroy();
    Send::g_send.reset();
}


