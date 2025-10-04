//IbSendMouse.cpp
#include"pch.h"
#include <Logitech.hpp>

// 全局系数缓存
static float g_mouseMoveCoefficient = 1.0f;

float GetMouseMoveCoefficient() {
	return g_mouseMoveCoefficient;
}


bool send_mouse_input_bulk(const MOUSEINPUT* inputs, uint32_t count) {
	auto& logitech = Send::Logitech::getLogitechInstance();
	for (uint32_t i = 0; i < count; ++i) {
		if (!logitech.send_mouse_report(inputs[i])) return false;
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

	//纠正系数
	double coeff = GetMouseMoveCoefficient();
	dx = static_cast<int32_t>(dx * coeff);
	dy = static_cast<int32_t>(dy * coeff);

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
	//高频连续调用MouseMoveAbsolute函数,GetCursorPos(&current_pos)中的数据来不及更新,因此计算的偏移会有偏差
	POINT current_pos;
	if (!GetCursorPos(&current_pos)) {
		return false;
	}

	int32_t dx = static_cast<int32_t>(target_x) - current_pos.x;
	int32_t dy = static_cast<int32_t>(target_y) - current_pos.y;

	return MouseMoveRelative(dx, dy);
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

DLLAPI void WINAPI SetMouseMoveCoefficient(float coefficient) {
	g_mouseMoveCoefficient = coefficient;
}

DLLAPI float WINAPI AutoCalibrate() {
	const int32_t testDx = 300;
	const int32_t testDy = 0;

	// 1. 保存用户当前鼠标位置
	POINT userPos;
	if (!GetCursorPos(&userPos)) {
		printf("无法获取鼠标位置\n");
		return 1.0f;
	}

	// 2. 移动鼠标到初始校准位置
	SetCursorPos(0, 0);
	POINT startPos;
	if (!GetCursorPos(&startPos)) {
		printf("无法获取校准初始位置\n");
		return 1.0f;
	}

	// 3. 重置自动系数
	SetMouseMoveCoefficient(1.0f);

	// 4. 原始移动
	MouseMoveRelative(testDx, testDy);
	Sleep(1);

	// 获取实际鼠标位置
	POINT endPos;
	if (!GetCursorPos(&endPos)) {
		printf("无法获取校准结束位置\n");
		SetCursorPos(userPos.x, userPos.y);
		return 1.0f;
	}

	// 检查鼠标是否移到屏幕边界
	if (endPos.x >= GetSystemMetrics(SM_CXSCREEN) - 1) {
		printf("鼠标灵敏度过高,自动校准失败,请手动设置系数\n");
		SetCursorPos(userPos.x, userPos.y);
		return 1.0f;
	}

	// 5. 计算实际偏移
	int32_t movedX = endPos.x - startPos.x;

	// 6. 计算系数
	float coeffX = (movedX != 0) ? static_cast<float>(testDx) / movedX : 1.0f;

	// 7. 还原用户鼠标位置
	SetCursorPos(userPos.x, userPos.y);

	printf("自动校准完成,鼠标移动系数: %.6f\n", coeffX); // 打印小数点后6位
	return coeffX;
}
