#include "InputSimulator.hpp"
using namespace Send;

#include "types.hpp"
#include "holder.hpp"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        IbSendDestroy();
        break;
    }
    return TRUE;
}

namespace main {
    static std::unique_ptr<Type::Base> send;
}

class SendInputHook {
public:
    static inline decltype(SendInput)* SendInput_real = SendInput;
    static UINT WINAPI SendInput_detour(UINT cInputs, LPINPUT pInputs, int cbSize) {
        if (!hook)
            return SendInput_real(cInputs, pInputs, cbSize);

        return main::send->send_input(pInputs, cInputs);
    }

    //#TODO: only needed when two or more AHK processes exist?
    static inline decltype(GetAsyncKeyState)* GetAsyncKeyState_real = GetAsyncKeyState;
    static SHORT WINAPI GetAsyncKeyState_detour(int vKey) {
        if (!hook)
            return GetAsyncKeyState_real(vKey);

        //if constexpr (debug)
        //    DebugOStream() << L"GetAsyncKeyState: " << vKey << ", " << main::send->get_key_state(vKey) << std::endl;
        return main::send->get_key_state(vKey);
    }

public:
    static inline bool hook;

    SendInputHook() {
        hook = false;
        IbDetourAttach(&GetAsyncKeyState_real, GetAsyncKeyState_detour);
        IbDetourAttach(&SendInput_real, SendInput_detour);
    }

    ~SendInputHook() {
        IbDetourDetach(&SendInput_real, SendInput_detour);
        IbDetourDetach(&GetAsyncKeyState_real, GetAsyncKeyState_detour);
    }
};
ib::HolderB<SendInputHook> sendinput_hook;

DLLAPI void __stdcall IbSendInputHook(HookCode code) {
    switch (code) {
    case HookCode::InitOnly:
        sendinput_hook.create();
        break;
    case HookCode::Destroy:
        sendinput_hook.destroy();
        break;
    case HookCode::On:
        if (!sendinput_hook.created())
            sendinput_hook.create();
        sendinput_hook->hook = true;
        break;
    case HookCode::Off:
        sendinput_hook->hook = false;
        break;
    }
}


DLLAPI Send::Error __stdcall IbSendInit(SendType type, InitFlags flags, void* argument) {
    if (type != SendType::Logitech) {
        return Error::InvalidArgument;  // 只支持 Logitech
    }

    auto logitech = std::make_unique<Type::Logitech>();
    logitech->create_base(&SendInputHook::GetAsyncKeyState_real);
    Error error = logitech->create();
    if (error != Error::Success)
        return error;

    main::send = std::move(logitech);
    return Error::Success;
}



DLLAPI void __stdcall IbSendDestroy() {
    IbSendInputHook(HookCode::Destroy);

    if (!main::send)
        return;
    main::send->destroy();
    main::send.reset();
}

DLLAPI void __stdcall IbSendSyncKeyStates() {
    main::send->sync_key_states();
}

DLLAPI UINT WINAPI IbSendInput(
    _In_ UINT cInputs,
    _In_reads_(cInputs) LPINPUT pInputs,
    _In_ int cbSize
) {
    return main::send->send_input(pInputs, cInputs);
}