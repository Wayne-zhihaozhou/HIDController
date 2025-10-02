//ibsendkeyboard.cpp
#include"pch.h"
#include <bit>
#include <cassert>
#include <IbSendInput.hpp>
#include "IbSendKeyboard.hpp"

namespace Send {

	// 发送一个单独的键盘事件（相当于原生 keybd_event）
	DLLAPI VOID WINAPI IbSend_keybd_event(
		_In_ BYTE bVk,
		_In_ BYTE bScan,
		_In_ DWORD dwFlags,
		_In_ ULONG_PTR dwExtraInfo
	) {
		// 构造一个键盘输入结构
		INPUT input{
			.type = INPUT_KEYBOARD,
			.ki {
				.wVk = bVk,
				.wScan = bScan,
				.dwFlags = dwFlags,
				.time = 0,
				.dwExtraInfo = dwExtraInfo
			}
		};
		// 调用发送函数
		IbSendInput(1, &input, sizeof(INPUT));
	}

	// 模拟按下一个键
	DLLAPI bool WINAPI IbSendKeybdDown(uint16_t vk) {
		// 构造“按下”事件
		INPUT input{
			.type = INPUT_KEYBOARD,
			.ki {
				.wVk = vk,
				.wScan = 0,
				.dwFlags = 0,
				.time = 0,
				.dwExtraInfo = 0
			}
		};
		// 发送事件
		return IbSendInput(1, &input, sizeof(INPUT));
	}

	// 模拟释放一个键
	DLLAPI bool WINAPI IbSendKeybdUp(uint16_t vk) {
		// 构造“释放”事件
		INPUT input{
			.type = INPUT_KEYBOARD,
			.ki {
				.wVk = vk,
				.wScan = 0,
				.dwFlags = KEYEVENTF_KEYUP,
				.time = 0,
				.dwExtraInfo = 0
			}
		};
		// 发送事件
		return IbSendInput(1, &input, sizeof(INPUT));
	}

	// 模拟按下并释放一个键，可选带修饰键（Ctrl/Shift/Alt/Win）
	DLLAPI bool WINAPI IbSendKeybdDownUp(uint16_t vk, Send::KeyboardModifiers modifiers) {
		// 初始化空的输入结构
		constexpr INPUT blank_input = INPUT{ .type = INPUT_KEYBOARD, .ki { 0, 0, 0, 0, 0 } };

		// 预留输入数组
		INPUT inputs[18];  // 最多支持8个修饰键 + 按下 + 抬起
		uint32_t i = 0;

		// 先按下所有启用的修饰键
#define CODE_GENERATE(vk, member)  \
    if (modifiers.member) {  \
        inputs[i] = blank_input;  \
        inputs[i].ki.wVk = vk;  \
        i++;  \
    }

		CODE_GENERATE(VK_LCONTROL, LCtrl)
			CODE_GENERATE(VK_RCONTROL, RCtrl)
			CODE_GENERATE(VK_LSHIFT, LShift)
			CODE_GENERATE(VK_RSHIFT, RShift)
			CODE_GENERATE(VK_LMENU, LAlt)
			CODE_GENERATE(VK_RMENU, RAlt)
			CODE_GENERATE(VK_LWIN, LWin)
			CODE_GENERATE(VK_RWIN, RWin)
#undef CODE_GENERATE

			// 按下目标键
			inputs[i] = blank_input;
		inputs[i].ki.wVk = vk;
		i++;

		// 释放目标键
		inputs[i] = inputs[i - 1];
		inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;
		i++;

		// 释放所有修饰键（逆序释放）
#define CODE_GENERATE(vk, member)  \
    if (modifiers.member) {  \
        inputs[i] = blank_input;  \
        inputs[i].ki.wVk = vk;  \
        inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;  \
        i++;  \
    }

		CODE_GENERATE(VK_RWIN, RWin)
			CODE_GENERATE(VK_LWIN, LWin)
			CODE_GENERATE(VK_RMENU, RAlt)
			CODE_GENERATE(VK_LMENU, LAlt)
			CODE_GENERATE(VK_RSHIFT, RShift)
			CODE_GENERATE(VK_LSHIFT, LShift)
			CODE_GENERATE(VK_RCONTROL, RCtrl)
			CODE_GENERATE(VK_LCONTROL, LCtrl)
#undef CODE_GENERATE

			// 一次性发送所有事件
			return IbSendInput(i, inputs, sizeof(INPUT)) == i;
	}

}
