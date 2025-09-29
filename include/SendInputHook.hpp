#pragma once
#include "globals.hpp"
#include "holder.hpp"
#include "detours.h"
#include <Windows.h>

template<typename T>
LONG IbDetourAttach(_Inout_ T* ppPointer, _In_ T pDetour) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)ppPointer, pDetour);
    return DetourTransactionCommit();
}

template<typename T>
LONG IbDetourDetach(_Inout_ T* ppPointer, _In_ T pDetour) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach((void**)ppPointer, pDetour);
    return DetourTransactionCommit();
}

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
