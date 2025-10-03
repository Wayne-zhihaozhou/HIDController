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


//待修改确认.
DLLAPI bool WINAPI MouseMoveAbsolute(uint32_t x, uint32_t y) {
	const uint32_t MAX_DELTA = 128;  // 分步移动最大增量

	POINT curr{};
	if (!GetCursorPos(&curr)) return false;

	int32_t dx = static_cast<int32_t>(x) - curr.x;
	int32_t dy = static_cast<int32_t>(y) - curr.y;

	int32_t steps = max(
		(std::abs(dx) + MAX_DELTA - 1) / MAX_DELTA,
		(std::abs(dy) + MAX_DELTA - 1) / MAX_DELTA
	);
	if (steps == 0) steps = 1;

	std::vector<MOUSEINPUT> moves;
	moves.reserve(steps);

	float step_x = static_cast<float>(dx) / steps;
	float step_y = static_cast<float>(dy) / steps;
	float prev_x = static_cast<float>(curr.x);
	float prev_y = static_cast<float>(curr.y);

	for (int32_t i = 1; i <= steps; ++i) {
		float curr_step_x = curr.x + step_x * i;
		float curr_step_y = curr.y + step_y * i;

		MOUSEINPUT mi{};
		mi.dx = static_cast<int32_t>(curr_step_x + 0.5f);
		mi.dy = static_cast<int32_t>(curr_step_y + 0.5f);
		mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;

		moves.push_back(mi);
		prev_x = curr_step_x;
		prev_y = curr_step_y;
	}

	return send_mouse_input_bulk(moves.data(), static_cast<uint32_t>(moves.size()));
}



// 模拟一次鼠标点击（按下 + 抬起）
DLLAPI bool WINAPI MouseClick(Send::MouseButton button) {
	// 初始化按下和抬起事件
	INPUT inputs[2];
	inputs[0] = inputs[1] = {
		.type = INPUT_MOUSE,
		.mi {
			.dx = 0,
			.dy = 0,
			.mouseData = 0,
			.time = 0,
			.dwExtraInfo = 0
		}
	};

	// 根据按键类型设置事件标志
	switch (button) {
	case Send::MouseButton::Left:
		inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
		return IbSendInput(2, inputs, sizeof(INPUT)) == 2;
	case Send::MouseButton::Right:
		inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		return IbSendInput(2, inputs, sizeof(INPUT)) == 2;
	case Send::MouseButton::Middle:
		inputs[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
		inputs[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
		return IbSendInput(2, inputs, sizeof(INPUT)) == 2;
	case Send::MouseButton::XButton1:
		inputs[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
		inputs[1].mi.dwFlags = MOUSEEVENTF_XUP;
		inputs[0].mi.mouseData = inputs[1].mi.mouseData = XBUTTON1;
		return IbSendInput(2, inputs, sizeof(INPUT)) == 2;
	case Send::MouseButton::XButton2:
		inputs[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
		inputs[1].mi.dwFlags = MOUSEEVENTF_XUP;
		inputs[0].mi.mouseData = inputs[1].mi.mouseData = XBUTTON2;
		return IbSendInput(2, inputs, sizeof(INPUT)) == 2;
	default:
		// 默认处理自定义标志
		inputs[0].mi.dwFlags = static_cast<DWORD>(button);
		return IbSendInput(1, inputs, sizeof(INPUT));
	}
}


// 模拟鼠标滚轮滚动
DLLAPI bool WINAPI MouseWheel(int32_t movement) {
	// 构造滚轮事件
	INPUT input{
		.type = INPUT_MOUSE,
		.mi {
			.dx = 0,
			.dy = 0,
			.mouseData = std::bit_cast<DWORD>(movement),
			.dwFlags = MOUSEEVENTF_WHEEL,
			.time = 0,
			.dwExtraInfo = 0
		}
	};
	// 发送事件
	return IbSendInput(1, &input, sizeof(INPUT));
}

