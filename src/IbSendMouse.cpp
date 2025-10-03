//IbSendMouse.cpp
#include"pch.h"
#include <Logitech.hpp>
#include <SendInputHook.hpp>



bool send_mouse_input_bulk(const MOUSEINPUT* inputs, uint32_t count) {
	auto logitech = std::make_unique<Send::Internal::Logitech>();
	logitech->create();
	for (uint32_t i = 0; i < count; ++i) {
		if (!logitech->send_mouse_report(inputs[i])) return false;
	}
	return true;
}

DLLAPI bool WINAPI MouseDown(Send::MouseButton button) {
	MOUSEINPUT input{};
	input.dx = input.dy = 0;
	input.mouseData = 0;
	input.dwExtraInfo = 0;
	input.time = 0;

	switch (button) {
	case Send::MouseButton::Left:   input.dwFlags = MOUSEEVENTF_LEFTDOWN; break;
	case Send::MouseButton::Right:  input.dwFlags = MOUSEEVENTF_RIGHTDOWN; break;
	case Send::MouseButton::Middle: input.dwFlags = MOUSEEVENTF_MIDDLEDOWN; break;
	case Send::MouseButton::XButton1: input.dwFlags = MOUSEEVENTF_XDOWN; input.mouseData = XBUTTON1; break;
	case Send::MouseButton::XButton2: input.dwFlags = MOUSEEVENTF_XDOWN; input.mouseData = XBUTTON2; break;
	default:
		input.dwFlags = static_cast<DWORD>(button);
		break;
	}

	return send_mouse_input_bulk(&input, 1);
}

DLLAPI bool WINAPI MouseUp(Send::MouseButton button) {
	MOUSEINPUT input{};
	input.dx = input.dy = 0;
	input.mouseData = 0;
	input.dwExtraInfo = 0;
	input.time = 0;

	switch (button) {
	case Send::MouseButton::Left:   input.dwFlags = MOUSEEVENTF_LEFTUP; break;
	case Send::MouseButton::Right:  input.dwFlags = MOUSEEVENTF_RIGHTUP; break;
	case Send::MouseButton::Middle: input.dwFlags = MOUSEEVENTF_MIDDLEUP; break;
	case Send::MouseButton::XButton1: input.dwFlags = MOUSEEVENTF_XUP; input.mouseData = XBUTTON1; break;
	case Send::MouseButton::XButton2: input.dwFlags = MOUSEEVENTF_XUP; input.mouseData = XBUTTON2; break;
	default:
		input.dwFlags = static_cast<DWORD>(button);
		break;
	}

	return send_mouse_input_bulk(&input, 1);
}

DLLAPI bool WINAPI MouseClick(Send::MouseButton button) {
	MOUSEINPUT inputs[2]{};

	// 初始化 down/up
	inputs[0].dx = inputs[1].dx = 0;
	inputs[0].dy = inputs[1].dy = 0;
	inputs[0].mouseData = inputs[1].mouseData = 0;
	inputs[0].dwExtraInfo = inputs[1].dwExtraInfo = 0;
	inputs[0].time = inputs[1].time = 0;

	switch (button) {
	case Send::MouseButton::Left:
		inputs[0].dwFlags = MOUSEEVENTF_LEFTDOWN;
		inputs[1].dwFlags = MOUSEEVENTF_LEFTUP;
		break;
	case Send::MouseButton::Right:
		inputs[0].dwFlags = MOUSEEVENTF_RIGHTDOWN;
		inputs[1].dwFlags = MOUSEEVENTF_RIGHTUP;
		break;
	case Send::MouseButton::Middle:
		inputs[0].dwFlags = MOUSEEVENTF_MIDDLEDOWN;
		inputs[1].dwFlags = MOUSEEVENTF_MIDDLEUP;
		break;
	case Send::MouseButton::XButton1:
		inputs[0].dwFlags = MOUSEEVENTF_XDOWN;
		inputs[1].dwFlags = MOUSEEVENTF_XUP;
		inputs[0].mouseData = inputs[1].mouseData = XBUTTON1;
		break;
	case Send::MouseButton::XButton2:
		inputs[0].dwFlags = MOUSEEVENTF_XDOWN;
		inputs[1].dwFlags = MOUSEEVENTF_XUP;
		inputs[0].mouseData = inputs[1].mouseData = XBUTTON2;
		break;
	default:
		inputs[0].dwFlags = static_cast<DWORD>(button);
		return send_mouse_input_bulk(&inputs[0], 1);
	}

	return send_mouse_input_bulk(inputs, 2);
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



