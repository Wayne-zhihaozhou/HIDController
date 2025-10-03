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

// 只按下鼠标按钮
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

// 只松开鼠标按钮
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

// 按下然后松开（完整点击）
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


DLLAPI bool WINAPI MouseMoveAbsolute(uint32_t target_x, uint32_t target_y) {
	// 获取屏幕分辨率
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	// 坐标映射到 HID 逻辑坐标 [0,65535]
	auto map_to_absolute = [](uint32_t value, int max) -> LONG {
		return static_cast<LONG>((value * 65535) / max);
		};

	LONG abs_x = map_to_absolute(target_x, screen_width - 1);
	LONG abs_y = map_to_absolute(target_y, screen_height - 1);

	MOUSEINPUT mi{};
	mi.dx = abs_x;
	mi.dy = abs_y;
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


//待修改确认.
//DLLAPI bool WINAPI MouseMoveAbsolute(uint32_t x, uint32_t y) {
//	const uint32_t MAX_DELTA = 128;  // 分步移动最大增量
//
//	POINT curr{};
//	if (!GetCursorPos(&curr)) return false;
//
//	int32_t dx = static_cast<int32_t>(x) - curr.x;
//	int32_t dy = static_cast<int32_t>(y) - curr.y;
//
//	int32_t steps = max(
//		(std::abs(dx) + MAX_DELTA - 1) / MAX_DELTA,
//		(std::abs(dy) + MAX_DELTA - 1) / MAX_DELTA
//	);
//	if (steps == 0) steps = 1;
//
//	std::vector<MOUSEINPUT> moves;
//	moves.reserve(steps);
//
//	float step_x = static_cast<float>(dx) / steps;
//	float step_y = static_cast<float>(dy) / steps;
//	float prev_x = static_cast<float>(curr.x);
//	float prev_y = static_cast<float>(curr.y);
//
//	for (int32_t i = 1; i <= steps; ++i) {
//		float curr_step_x = curr.x + step_x * i;
//		float curr_step_y = curr.y + step_y * i;
//
//		MOUSEINPUT mi{};
//		mi.dx = static_cast<int32_t>(curr_step_x + 0.5f);
//		mi.dy = static_cast<int32_t>(curr_step_y + 0.5f);
//		mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
//
//		moves.push_back(mi);
//		prev_x = curr_step_x;
//		prev_y = curr_step_y;
//	}
//
//	return send_mouse_input_bulk(moves.data(), static_cast<uint32_t>(moves.size()));
//}



//待修改
// 模拟一次鼠标点击（按下 + 抬起）
//DLLAPI bool WINAPI MouseClick(Send::MouseButton button) {
//	// 初始化按下和抬起事件
//	INPUT inputs[2];
//	inputs[0] = inputs[1] = {
//		.type = INPUT_MOUSE,
//		.mi {
//			.dx = 0,
//			.dy = 0,
//			.mouseData = 0,
//			.time = 0,
//			.dwExtraInfo = 0
//		}
//	};
//
//	// 根据按键类型设置事件标志
//	switch (button) {
//	case Send::MouseButton::Left:
//		inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
//		inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
//		return IbSendInput(2, inputs, sizeof(INPUT)) == 2;
//	case Send::MouseButton::Right:
//		inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
//		inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
//		return IbSendInput(2, inputs, sizeof(INPUT)) == 2;
//	case Send::MouseButton::Middle:
//		inputs[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
//		inputs[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
//		return IbSendInput(2, inputs, sizeof(INPUT)) == 2;
//	case Send::MouseButton::XButton1:
//		inputs[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
//		inputs[1].mi.dwFlags = MOUSEEVENTF_XUP;
//		inputs[0].mi.mouseData = inputs[1].mi.mouseData = XBUTTON1;
//		return IbSendInput(2, inputs, sizeof(INPUT)) == 2;
//	case Send::MouseButton::XButton2:
//		inputs[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
//		inputs[1].mi.dwFlags = MOUSEEVENTF_XUP;
//		inputs[0].mi.mouseData = inputs[1].mi.mouseData = XBUTTON2;
//		return IbSendInput(2, inputs, sizeof(INPUT)) == 2;
//	default:
//		// 默认处理自定义标志
//		inputs[0].mi.dwFlags = static_cast<DWORD>(button);
//		return IbSendInput(1, inputs, sizeof(INPUT));
//	}
//}



