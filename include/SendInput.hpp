//IbSendInput.hpp
#pragma once
#include "SendTypes.hpp"

#ifdef __cplusplus
extern "C" {
#endif

	// 发送输入事件
	UINT WINAPI IbSendInput(
		_In_ UINT cInputs,
		_In_reads_(cInputs) LPINPUT pInputs,
		_In_ int cbSize
	);

	// 安装或触发输入钩子
	void WINAPI IbSendInputHook(Send::HookCode code);

#ifdef __cplusplus
}
#endif




