class SendInputHook {
public:
    static inline decltype(SendInput)* SendInput_real = SendInput; // 原始 SendInput
    static UINT WINAPI SendInput_detour(UINT cInputs, LPINPUT pInputs, int cbSize) {
        if (!hook)
            return SendInput_real(cInputs, pInputs, cbSize);
        return main::send->send_input(pInputs, cInputs); // 重定向输入
    }

    static inline decltype(GetAsyncKeyState)* GetAsyncKeyState_real = GetAsyncKeyState; // 原始 GetAsyncKeyState
    static SHORT WINAPI GetAsyncKeyState_detour(int vKey) {
        if (!hook)
            return GetAsyncKeyState_real(vKey);
        return main::send->get_key_state(vKey); // 获取按键状态
    }

public:
    static inline bool hook;

    SendInputHook() {
        hook = false;
        IbDetourAttach(&GetAsyncKeyState_real, GetAsyncKeyState_detour); // 安装钩子
        IbDetourAttach(&SendInput_real, SendInput_detour);
    }

    ~SendInputHook() {
        IbDetourDetach(&SendInput_real, SendInput_detour); // 卸载钩子
        IbDetourDetach(&GetAsyncKeyState_real, GetAsyncKeyState_detour);
    }
};
ib::HolderB<SendInputHook> sendinput_hook; // 钩子管理
