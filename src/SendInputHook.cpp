// SendInputHook.cpp
#include "SendInputHook.hpp"

// 初始化静态成员
decltype(SendInputHook::SendInput_real) SendInputHook::SendInput_real = SendInput;
decltype(SendInputHook::GetAsyncKeyState_real) SendInputHook::GetAsyncKeyState_real = GetAsyncKeyState;
bool SendInputHook::hook = false;

UINT WINAPI SendInputHook::SendInput_detour(UINT cInputs, LPINPUT pInputs, int cbSize) {
    if (!hook)
        return SendInput_real(cInputs, pInputs, cbSize);
    return main::send->send_input(pInputs, cInputs);
}

SHORT WINAPI SendInputHook::GetAsyncKeyState_detour(int vKey) {
    if (!hook)
        return GetAsyncKeyState_real(vKey);
    return main::send->get_key_state(vKey);
}

SendInputHook::SendInputHook() {
    hook = false;
    IbDetourAttach(&GetAsyncKeyState_real, GetAsyncKeyState_detour);
    IbDetourAttach(&SendInput_real, SendInput_detour);
}

SendInputHook::~SendInputHook() {
    IbDetourDetach(&SendInput_real, SendInput_detour);
    IbDetourDetach(&GetAsyncKeyState_real, GetAsyncKeyState_detour);
}
