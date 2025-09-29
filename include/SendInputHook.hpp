#pragma once
#include "globals.hpp"
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
    static inline bool hook = false;
    static inline decltype(GetAsyncKeyState)* GetAsyncKeyState_real;

private:
    static inline decltype(SendInput)* SendInput_real;
    
    
};




// 全局钩子实例指针（生命周期由 IbSendInputHook 控制）
extern std::unique_ptr<SendInputHook> g_sendInputHook;
