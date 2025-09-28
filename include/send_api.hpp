DLLAPI void __stdcall IbSendSyncKeyStates() {
    main::send->sync_key_states(); // 同步按键状态
}

DLLAPI UINT WINAPI IbSendInput(
    _In_ UINT cInputs,
    _In_reads_(cInputs) LPINPUT pInputs,
    _In_ int cbSize
) {
    return main::send->send_input(pInputs, cInputs); // 调用发送函数
}
