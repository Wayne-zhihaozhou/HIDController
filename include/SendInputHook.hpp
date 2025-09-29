#pragma once
#include "globals.hpp"
#include "holder.hpp"

class SendInputHook {
public:
    SendInputHook();
    ~SendInputHook();

    static UINT WINAPI SendInput_detour(UINT cInputs, LPINPUT pInputs, int cbSize);
    static SHORT WINAPI GetAsyncKeyState_detour(int vKey);

private:
    static inline decltype(SendInput)* SendInput_real;
    static inline decltype(GetAsyncKeyState)* GetAsyncKeyState_real;
    static inline bool hook;
};




//待分类
ib::HolderB<SendInputHook> sendinput_hook; // 钩子管理
