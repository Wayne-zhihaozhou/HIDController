// ib_send_mouse.cpp - Logitech mouse control
#include "pch.h"
#include <logitech.h>

// 全局系数缓存
static float mouse_move_coefficient_ = 1.0f;
int original_params_[3];
int original_speed_;

float get_mouse_move_coefficient() {
	return mouse_move_coefficient_;
}

void backup_mouse_settings() {
	SystemParametersInfo(SPI_GETMOUSE, 0, original_params_, 0);
	SystemParametersInfo(SPI_GETMOUSESPEED, 0, &original_speed_, 0);
}

bool send_mouse_input_bulk(const MOUSEINPUT* inputs, uint32_t count) {
	auto& logitech = send::Logitech::get_logitech_instance();
	for (uint32_t i = 0; i < count; ++i) {
		if (!logitech.send_mouse_report(inputs[i])) return false;
	}
	return true;
}

DLLAPI bool WINAPI mouse_down(uint16_t button) {
	MOUSEINPUT mi{};
	mi.dx = 0;
	mi.dy = 0;
	mi.dwFlags = 0;
	mi.mouseData = 0;
	mi.time = 0;
	mi.dwExtraInfo = 0;

	// 左/右/中键
	if (button & MOUSEEVENTF_LEFTDOWN)   mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
	if (button & MOUSEEVENTF_RIGHTDOWN)  mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
	if (button & MOUSEEVENTF_MIDDLEDOWN) mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;

	// XButton
	if (button & MOUSEEVENTF_XDOWN) {
		mi.dwFlags |= MOUSEEVENTF_XDOWN;
		mi.mouseData = 0;
		if (button & XBUTTON1) mi.mouseData |= XBUTTON1;
		if (button & XBUTTON2) mi.mouseData |= XBUTTON2;
	}

	return send_mouse_input_bulk(&mi, 1);
}

DLLAPI bool WINAPI mouse_up(uint16_t button) {
	MOUSEINPUT mi{};
	mi.dx = 0;
	mi.dy = 0;
	mi.dwFlags = 0;
	mi.mouseData = 0;
	mi.time = 0;
	mi.dwExtraInfo = 0;

	// 左/右/中键
	if (button & MOUSEEVENTF_LEFTUP)   mi.dwFlags |= MOUSEEVENTF_LEFTUP;
	if (button & MOUSEEVENTF_RIGHTUP)  mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
	if (button & MOUSEEVENTF_MIDDLEUP) mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;

	// XButton
	if (button & MOUSEEVENTF_XUP) {
		mi.dwFlags |= MOUSEEVENTF_XUP;
		mi.mouseData = 0;
		if (button & XBUTTON1) mi.mouseData |= XBUTTON1;
		if (button & XBUTTON2) mi.mouseData |= XBUTTON2;
	}

	return send_mouse_input_bulk(&mi, 1);
}

DLLAPI bool WINAPI mouse_click(uint16_t button) {
	MOUSEINPUT inputs[2]{};

	// 初始化 MOUSEINPUT
	for (int i = 0; i < 2; ++i) {
		inputs[i].dx = 0;
		inputs[i].dy = 0;
		inputs[i].mouseData = 0;
		inputs[i].dwFlags = 0;
		inputs[i].dwExtraInfo = 0;
		inputs[i].time = 0;
	}

	// ---------------- XButton1/2 ----------------
	if (button & (MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP)) {
		inputs[0].dwFlags = button & (MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP);

		// 获取 XButton1 或 XButton2
		uint16_t xbtn = button & (XBUTTON1 | XBUTTON2);
		inputs[0].mouseData = xbtn;

		// 如果是 DOWN，自动生成 UP
		if (inputs[0].dwFlags & MOUSEEVENTF_XDOWN) {
			inputs[1].dwFlags = MOUSEEVENTF_XUP;
			inputs[1].mouseData = xbtn;
			return send_mouse_input_bulk(inputs, 2);
		}

		return send_mouse_input_bulk(&inputs[0], 1);
	}

	// ---------------- 左/右/中键 ----------------
	if (button & (MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_MIDDLEDOWN)) {
		inputs[0].dwFlags = button;  // DOWN

		// 自动生成 UP
		if (button == MOUSEEVENTF_LEFTDOWN) inputs[1].dwFlags = MOUSEEVENTF_LEFTUP;
		else if (button == MOUSEEVENTF_RIGHTDOWN) inputs[1].dwFlags = MOUSEEVENTF_RIGHTUP;
		else if (button == MOUSEEVENTF_MIDDLEDOWN) inputs[1].dwFlags = MOUSEEVENTF_MIDDLEUP;

		return send_mouse_input_bulk(inputs, 2);
	}

	// ---------------- 其他情况（单独 UP 或其他宏） ----------------
	inputs[0].dwFlags = button;
	return send_mouse_input_bulk(&inputs[0], 1);
}

DLLAPI bool WINAPI mouse_move_relative(int32_t dx, int32_t dy) {
	constexpr int32_t kMaxDelta = 128;

	//纠正系数
	float coeff = get_mouse_move_coefficient();
	dx = static_cast<int32_t>(dx * coeff);
	dy = static_cast<int32_t>(dy * coeff);

	int32_t steps = max(
		(std::abs(dx) + kMaxDelta - 1) / kMaxDelta,
		(std::abs(dy) + kMaxDelta - 1) / kMaxDelta
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

DLLAPI bool WINAPI mouse_move_absolute(uint32_t target_x, uint32_t target_y) {
	//高频连续调用MouseMoveAbsolute函数,GetCursorPos(&current_pos)中的数据来不及更新,因此计算的偏移会有偏差
	POINT current_pos;
	if (!GetCursorPos(&current_pos)) {
		return false;
	}

	int32_t dx = static_cast<int32_t>(target_x) - current_pos.x;
	int32_t dy = static_cast<int32_t>(target_y) - current_pos.y;

	return mouse_move_relative(dx, dy);
}

DLLAPI bool WINAPI mouse_wheel(int32_t movement) {
	constexpr int32_t kMaxDelta = 120;  // 每个 HID 报告最大滚动量，标准滚轮为 120

	int32_t steps = (std::abs(movement) + kMaxDelta - 1) / kMaxDelta;
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

DLLAPI void WINAPI set_mouse_move_coefficient(float coefficient) {
	mouse_move_coefficient_ = coefficient;
}

DLLAPI void WINAPI auto_calibrate() {
	const int32_t k_test_dx = 300;
	const int32_t k_test_dy = 0;

	// 1. 保存用户当前鼠标位置
	POINT user_pos;
	GetCursorPos(&user_pos);

	// 2. 移动鼠标到初始校准位置
	SetCursorPos(0, 0);
	POINT start_pos;
	GetCursorPos(&start_pos);

	// 3. 重置自动系数
	set_mouse_move_coefficient(1.0f);

	// 4. 原始移动
	mouse_move_relative(k_test_dx, k_test_dy);
	Sleep(1);

	// 获取实际鼠标位置
	POINT end_pos;
	GetCursorPos(&end_pos);

	// 检查鼠标是否移到屏幕边界
	if (end_pos.x >= GetSystemMetrics(SM_CXSCREEN) - 1) {
		printf("鼠标灵敏度过高,自动校准失败,请手动设置系数\n");
		SetCursorPos(user_pos.x, user_pos.y);
	}

	// 5. 计算实际偏移
	int32_t moved_x = end_pos.x - start_pos.x;

	// 6. 计算系数
	float coeff_x = (moved_x != 0) ? static_cast<float>(k_test_dx) / moved_x : 1.0f;

	// 7. 还原用户鼠标位置
	SetCursorPos(user_pos.x, user_pos.y);

	// 8. 保存系数
	set_mouse_move_coefficient(coeff_x);
	printf("自动校准成功,系数为%f\n", coeff_x);
}

DLLAPI void WINAPI disable_mouse_acceleration() {
	// 保存原始设置
	backup_mouse_settings();
	// 禁用鼠标加速(系统设置)
	int mouseParams[3] = { 0, 0, 0 };
	SystemParametersInfo(SPI_SETMOUSE, 0, mouseParams, SPIF_SENDCHANGE);
	int speed = 10;//（范围 1-20）
	SystemParametersInfo(SPI_SETMOUSESPEED, 0, &speed, SPIF_SENDCHANGE);
	printf("禁用鼠标加速成功\n");
}

DLLAPI void WINAPI enable_mouse_acceleration() {
	SystemParametersInfo(SPI_SETMOUSE, 0, original_params_, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETMOUSESPEED, 0, &original_speed_, SPIF_SENDCHANGE);
	printf("恢复鼠标设置成功\n");
}
