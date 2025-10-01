#include "dll_export.hpp"
#include "base.hpp"
#include <SendTypes.hpp>
#include <SendInputHook.hpp>


// 发送一组模拟输入事件（键盘/鼠标）
DLLAPI UINT WINAPI IbSendInput(
	_In_ UINT cInputs,
	_In_reads_(cInputs) LPINPUT pInputs,
	_In_ int cbSize
) {
	// 调用底层发送对象的输入函数
	return Send::g_send->send_input(pInputs, cInputs);
}


// 管理输入钩子的生命周期和启用状态
DLLAPI void __stdcall IbSendInputHook(Send::HookCode code) {
	switch (code) {
	case Send::HookCode::InitOnly:
		// 如果钩子未创建则初始化
		if (!g_sendInputHook)
			g_sendInputHook = std::make_unique<SendInputHook>();
		break;

	case Send::HookCode::Destroy:
		// 销毁钩子对象
		g_sendInputHook.reset();
		break;

	case Send::HookCode::On:
		// 初始化（若未创建）并启用钩子
		if (!g_sendInputHook)
			g_sendInputHook = std::make_unique<SendInputHook>();
		SendInputHook::hook = true;
		break;

	case Send::HookCode::Off:
		// 禁用钩子
		SendInputHook::hook = false;
		break;
	}
}
