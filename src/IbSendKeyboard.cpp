//ibsendkeyboard.cpp
#include"pch.h"
#include <Logitech.hpp>

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


bool send_keyboard_input_bulk(const KEYBDINPUT* inputs, uint32_t count) {
	auto logitech = std::make_unique<Send::Internal::Logitech>();
	logitech->create();
	for (uint32_t i = 0; i < count; ++i) {
		if (!logitech->send_keyboard_report(inputs[i])) return false;
	}
	return true;
}



// 模拟单个按键按下
DLLAPI bool WINAPI KeyDown(uint16_t vk) {
	KEYBDINPUT ki{};
	ki.wVk = vk;
	ki.dwFlags = 0;  // 按下
	ki.wScan = 0;
	ki.time = 0;
	ki.dwExtraInfo = 0;

	return send_keyboard_input_bulk(&ki, 1);
}

// 模拟单个按键抬起
DLLAPI bool WINAPI KeyUp(uint16_t vk) {
	KEYBDINPUT ki{};
	ki.wVk = vk;
	ki.dwFlags = KEYEVENTF_KEYUP; // 抬起
	ki.wScan = 0;
	ki.time = 0;
	ki.dwExtraInfo = 0;

	return send_keyboard_input_bulk(&ki, 1);
}

// 批量发送组合键，比如 Ctrl+Shift+A
DLLAPI bool WINAPI KeyCombo(const std::vector<uint16_t>& keys) {
	std::vector<KEYBDINPUT> inputs;
	inputs.reserve(keys.size() * 2);

	// ✅ 先按下所有键（从前到后）
	for (auto vk : keys) {
		KEYBDINPUT ki{};
		ki.wVk = vk;
		ki.dwFlags = 0;  // 按下
		inputs.push_back(ki);
	}

	// ✅ 再“反向”抬起所有键（从后到前，保证修饰键最后释放）
	for (auto it = keys.rbegin(); it != keys.rend(); ++it) {
		KEYBDINPUT ki{};
		ki.wVk = *it;
		ki.dwFlags = KEYEVENTF_KEYUP; // 抬起
		inputs.push_back(ki);
	}

	return send_keyboard_input_bulk(inputs.data(), static_cast<uint32_t>(inputs.size()));
}


/*
// 模拟按下一个键
DLLAPI bool WINAPI KeyDown(uint16_t vk) {
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
DLLAPI bool WINAPI KeyUp(uint16_t vk) {
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

// 模拟按下一个键，并释放它
DLLAPI bool WINAPI KeyPress(uint16_t vk) {
	// 创建两个 INPUT 事件：按下和松开
	INPUT inputs[2]{};

	// 按下键
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = vk;
	// dwFlags 默认为 0，表示按下

	// 松开键
	inputs[1] = inputs[0];
	inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

	// 一次发送两个事件
	return IbSendInput(2, inputs, sizeof(INPUT)) == 2;
}

// 模拟按下并释放一个键，可选带修饰键（Ctrl/Shift/Alt/Win）
DLLAPI bool WINAPI KeyCombo(uint16_t vk, Send::KeyboardModifiers modifiers) {
	struct Modifier { uint16_t vk; bool active; };
	Modifier mods[] = {
		{ VK_LCONTROL, modifiers.LCtrl },
		{ VK_RCONTROL, modifiers.RCtrl },
		{ VK_LSHIFT, modifiers.LShift },
		{ VK_RSHIFT, modifiers.RShift },
		{ VK_LMENU, modifiers.LAlt },
		{ VK_RMENU, modifiers.RAlt },
		{ VK_LWIN, modifiers.LWin },
		{ VK_RWIN, modifiers.RWin },
	};

	INPUT inputs[18]{};
	int idx = 0;

	// 按下修饰键
	for (auto& m : mods) {
		if (m.active) {
			inputs[idx].type = INPUT_KEYBOARD;
			inputs[idx].ki.wVk = m.vk;
			idx++;
		}
	}

	// 按下目标键
	inputs[idx].type = INPUT_KEYBOARD;
	inputs[idx].ki.wVk = vk;
	idx++;

	// 松开目标键
	inputs[idx] = inputs[idx - 1];
	inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP;
	idx++;

	// 松开修饰键（逆序）
	for (int i = 7; i >= 0; i--) {
		if (mods[i].active) {
			inputs[idx].type = INPUT_KEYBOARD;
			inputs[idx].ki.wVk = mods[i].vk;
			inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP;
			idx++;
		}
	}

	return IbSendInput(idx, inputs, sizeof(INPUT)) == idx;
}
*/