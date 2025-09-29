// SendInputHook.cpp
#include "SendInputHook.hpp"

// 初始化全局智能指针
std::unique_ptr<SendInputHook> g_sendInputHook = nullptr;

// 初始化静态成员
//decltype(SendInputHook::SendInput_real) SendInputHook::SendInput_real = SendInput;
//decltype(SendInputHook::GetAsyncKeyState_real) SendInputHook::GetAsyncKeyState_real = GetAsyncKeyState;
//bool SendInputHook::hook = false;


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
    return main::send->get_key_state(vKey);
}

// 拦截 SendInput
UINT WINAPI SendInputHook::SendInput_detour(UINT cInputs, LPINPUT pInputs, int cbSize) {
    if (!hook)
        return SendInput_real(cInputs, pInputs, cbSize);
    return main::send->send_input(pInputs, cInputs);
}