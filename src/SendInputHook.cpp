// SendInputHook.cpp
#include"pch.h"
#include <SendInputHook.hpp>


// 初始化全局智能指针
std::unique_ptr<SendInputHook> g_sendInputHook = nullptr;

// 构造函数：安装 detour
SendInputHook::SendInputHook() {
    hook = false;
    IbDetourAttach(&GetAsyncKeyState_real, GetAsyncKeyState_detour);
    IbDetourAttach(&SendInput_real, SendInput_detour);
}

// 析构函数：卸载 detour
SendInputHook::~SendInputHook() {
    IbDetourDetach(&SendInput_real, SendInput_detour);
    IbDetourDetach(&GetAsyncKeyState_real, GetAsyncKeyState_detour);
}

// 拦截 GetAsyncKeyState
SHORT WINAPI SendInputHook::GetAsyncKeyState_detour(int vKey) {
    if (!hook)
        return GetAsyncKeyState_real(vKey);
    return Send::g_send->get_key_state(vKey);
}

// 拦截 SendInput
UINT WINAPI SendInputHook::SendInput_detour(UINT cInputs, LPINPUT pInputs, int cbSize) {
    if (!hook)
        return SendInput_real(cInputs, pInputs, cbSize);
    return Send::g_send->send_input(pInputs, cInputs);
}