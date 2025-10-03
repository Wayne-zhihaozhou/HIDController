//IbSendMouse.cpp
#include"pch.h"
#include <Logitech.hpp>



bool send_mouse_input_bulk(const MOUSEINPUT* inputs, uint32_t count) {
	auto logitech = std::make_unique<Send::Internal::Logitech>();
	logitech->create();
	for (uint32_t i = 0; i < count; ++i) {
		if (!logitech->send_mouse_report(inputs[i])) return false;
	}
	return true;
}

DLLAPI bool WINAPI MouseDown(uint16_t button) {
	MOUSEINPUT input{};
	input.dx = 0;
	input.dy = 0;
	input.dwExtraInfo = 0;
	input.time = 0;
	input.mouseData = 0;

	// 对于 XBUTTON1/XBUTTON2，需要设置 mouseData
	if (button & (MOUSEEVENTF_XDOWN)) {
		input.mouseData = (button & 0x100) ? XBUTTON1 : XBUTTON2;  // 0x100 表示 XBUTTON1, 0x200 表示 XBUTTON2
	}

	input.dwFlags = static_cast<DWORD>(button);

	return send_mouse_input_bulk(&input, 1);
}

DLLAPI bool WINAPI MouseUp(uint16_t button) {
	MOUSEINPUT input{};
	input.dx = 0;
	input.dy = 0;
	input.dwExtraInfo = 0;
	input.time = 0;
	input.mouseData = 0;

	// 对于 XBUTTON1/XBUTTON2，需要设置 mouseData
	if (button & (MOUSEEVENTF_XUP)) {
		input.mouseData = (button & 0x100) ? XBUTTON1 : XBUTTON2;
	}

	input.dwFlags = static_cast<DWORD>(button);

	return send_mouse_input_bulk(&input, 1);
}

DLLAPI bool WINAPI MouseClick(uint16_t button) {
	MOUSEINPUT inputs[2]{};

	// 初始化 down/up
	for (int i = 0; i < 2; ++i) {
		inputs[i].dx = 0;
		inputs[i].dy = 0;
		inputs[i].mouseData = 0;
		inputs[i].dwExtraInfo = 0;
		inputs[i].time = 0;
	}

	// X按钮特殊处理 mouseData
	if (button & (MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP)) {
		inputs[0].mouseData = inputs[1].mouseData = (button & 0x100) ? XBUTTON1 : XBUTTON2;
	}

	// 如果传入的是单个 DOWN/UP 就直接发送
	if (button & (MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_XDOWN)) {
		inputs[0].dwFlags = static_cast<DWORD>(button);  // DOWN
		inputs[1].dwFlags = 0;

		// 对应 UP
		if (button == MOUSEEVENTF_LEFTDOWN) inputs[1].dwFlags = MOUSEEVENTF_LEFTUP;
		else if (button == MOUSEEVENTF_RIGHTDOWN) inputs[1].dwFlags = MOUSEEVENTF_RIGHTUP;
		else if (button == MOUSEEVENTF_MIDDLEDOWN) inputs[1].dwFlags = MOUSEEVENTF_MIDDLEUP;
		else if (button & MOUSEEVENTF_XDOWN) inputs[1].dwFlags = MOUSEEVENTF_XUP;

		return send_mouse_input_bulk(inputs, 2);
	}

	// 如果直接传入的是 UP 或 其他宏，直接发送
	inputs[0].dwFlags = static_cast<DWORD>(button);
	return send_mouse_input_bulk(&inputs[0], 1);
}

DLLAPI bool WINAPI MouseMoveRelative(int32_t dx, int32_t dy) {
	const int32_t MAX_DELTA = 128;

	int32_t steps = max(
		(std::abs(dx) + MAX_DELTA - 1) / MAX_DELTA,
		(std::abs(dy) + MAX_DELTA - 1) / MAX_DELTA
	);
	if (steps == 0) steps = 1;

	std::vector<MOUSEINPUT> moves;
	moves.reserve(steps);

	float step_x = static_cast<float>(dx) / steps;
	float step_y = static_cast<float>(dy) / steps;
	float prev_x = 0, prev_y = 0;

	for (int32_t i = 1; i <= steps; ++i) {
		float curr_x = step_x * i;
		float curr_y = step_y * i;

		MOUSEINPUT mi{};
		mi.dx = static_cast<int32_t>(curr_x - prev_x + 0.5f);
		mi.dy = static_cast<int32_t>(curr_y - prev_y + 0.5f);
		mi.dwFlags = MOUSEEVENTF_MOVE;

		moves.push_back(mi);
		prev_x = curr_x;
		prev_y = curr_y;
	}

	return send_mouse_input_bulk(moves.data(), static_cast<uint32_t>(moves.size()));
}

DLLAPI bool WINAPI MouseMoveAbsolute(uint32_t target_x, uint32_t target_y) {
	// 获取屏幕分辨率
	//int screen_width = GetSystemMetrics(SM_CXSCREEN);
	//int screen_height = GetSystemMetrics(SM_CYSCREEN);

	//// 坐标映射到 HID 逻辑坐标 [0,65535]
	//auto map_to_absolute = [](uint32_t value, int max) -> LONG {
	//	return static_cast<LONG>((value * 65535) / max);
	//	};

	//LONG abs_x = map_to_absolute(target_x, screen_width - 1);
	//LONG abs_y = map_to_absolute(target_y, screen_height - 1);

	MOUSEINPUT mi{};
	mi.dx = 200;
	mi.dy = 200;
	mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	mi.time = 0;
	mi.dwExtraInfo = 0;

	// 创建 Logitech 对象并发送一次 HID 报告
	auto logitech = std::make_unique<Send::Internal::Logitech>();
	logitech->create();

	if (!logitech->send_mouse_report(mi)) {
		return false;
	}

	return true;
}

DLLAPI bool WINAPI MouseWheel(int32_t movement) {
	const int32_t MAX_DELTA = 120;  // 每个 HID 报告最大滚动量，标准滚轮为 120

	int32_t steps = (std::abs(movement) + MAX_DELTA - 1) / MAX_DELTA;
	if (steps == 0) steps = 1;

	std::vector<MOUSEINPUT> wheels;
	wheels.reserve(steps);

	float step_value = static_cast<float>(movement) / steps;
	float prev_value = 0;

	for (int32_t i = 1; i <= steps; ++i) {
		float curr_value = step_value * i;

		MOUSEINPUT mi{};
		mi.dx = 0;
		mi.dy = 0;
		mi.dwFlags = MOUSEEVENTF_WHEEL;
		mi.mouseData = static_cast<DWORD>(curr_value - prev_value + 0.5f);
		mi.time = 0;
		mi.dwExtraInfo = 0;

		wheels.push_back(mi);
		prev_value = curr_value;
	}

	return send_mouse_input_bulk(wheels.data(), static_cast<uint32_t>(wheels.size()));
}



