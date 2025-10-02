//IbSendBase.hpp
#pragma once
#include "SendTypes.hpp"

#ifdef __cplusplus
extern "C" {
#endif

	// 初始化发送模块
	Send::Error WINAPI IbSendInit(Send::SendType type, Send::InitFlags flags, void* argument);

	// 销毁发送模块
	void WINAPI IbSendDestroy();

	// 同步键盘状态
	void WINAPI IbSendSyncKeyStates();

#ifdef __cplusplus
}
#endif
