//SendInputHook.hpp
#pragma once
#include "detours.h"

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
    static inline decltype(GetAsyncKeyState)* GetAsyncKeyState_real = GetAsyncKeyState;

private:
    static inline decltype(SendInput)* SendInput_real = SendInput;
};

// 全局钩子实例
extern std::unique_ptr<SendInputHook> g_sendInputHook;
