#include "dll_export.hpp"
#include "globals.hpp"

DLLAPI UINT WINAPI IbSendInput(
    _In_ UINT cInputs,
    _In_reads_(cInputs) LPINPUT pInputs,
    _In_ int cbSize
) {
    return main::send->send_input(pInputs, cInputs); // 调用发送函数
}
