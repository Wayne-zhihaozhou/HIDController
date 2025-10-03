//ibsendkeyboard.cpp
#include"pch.h"
#include <Logitech.hpp>

bool send_keyboard_input_bulk(const KEYBDINPUT* inputs, uint32_t count) {
	auto logitech = std::make_unique<Send::Internal::Logitech>();
	logitech->create();
	for (uint32_t i = 0; i < count; ++i) {
		if (!logitech->send_keyboard_report(inputs[i])) return false;
	}
	return true;
}

DLLAPI bool WINAPI KeyDown(uint16_t vk) {
	KEYBDINPUT ki{};
	ki.wVk = vk;
	ki.dwFlags = 0;  // 按下
	ki.wScan = 0;
	ki.time = 0;
	ki.dwExtraInfo = 0;

	return send_keyboard_input_bulk(&ki, 1);
}

DLLAPI bool WINAPI KeyUp(uint16_t vk) {
	KEYBDINPUT ki{};
	ki.wVk = vk;
	ki.dwFlags = KEYEVENTF_KEYUP; // 抬起
	ki.wScan = 0;
	ki.time = 0;
	ki.dwExtraInfo = 0;

	return send_keyboard_input_bulk(&ki, 1);
}

DLLAPI bool WINAPI KeyPress(uint16_t vk) {
	// 构建一次性报告数组，先按下再松开
	KEYBDINPUT inputs[2]{};

	// 按下
	inputs[0].wVk = vk;
	inputs[0].dwFlags = 0;  // 按下
	inputs[0].wScan = 0;
	inputs[0].time = 0;
	inputs[0].dwExtraInfo = 0;

	// 抬起
	inputs[1].wVk = vk;
	inputs[1].dwFlags = KEYEVENTF_KEYUP;  // 抬起
	inputs[1].wScan = 0;
	inputs[1].time = 0;
	inputs[1].dwExtraInfo = 0;

	// 一次性发送所有报告
	return send_keyboard_input_bulk(inputs, 2);
}

DLLAPI bool WINAPI KeyCombo(const std::vector<uint16_t>& keys) {
	// 批量发送组合键，比如 Ctrl+Shift+A
	std::vector<KEYBDINPUT> inputs;
	inputs.reserve(keys.size() * 2);

	// 先按下所有键（从前到后）
	for (auto vk : keys) {
		KEYBDINPUT ki{};
		ki.wVk = vk;
		ki.dwFlags = 0;  // 按下
		inputs.push_back(ki);
	}

	// 再“反向”抬起所有键（从后到前，保证修饰键最后释放）
	for (auto it = keys.rbegin(); it != keys.rend(); ++it) {
		KEYBDINPUT ki{};
		ki.wVk = *it;
		ki.dwFlags = KEYEVENTF_KEYUP; // 抬起
		inputs.push_back(ki);
	}

	return send_keyboard_input_bulk(inputs.data(), static_cast<uint32_t>(inputs.size()));
}

