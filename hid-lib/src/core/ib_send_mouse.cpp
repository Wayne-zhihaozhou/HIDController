// ib_send_mouse.cpp - Logitech mouse control
#include "pch.h"
#include "logitech.h"

// 全局系数缓存
static float mouse_move_coefficient_ = 1.0f;
static int original_params_[3];
static int original_speed_;

DLLAPI float WINAPI get_mouse_move_coefficient() {
	return mouse_move_coefficient_;
}

static void backup_mouse_settings() {
	SystemParametersInfo(SPI_GETMOUSE, 0, original_params_, 0);
	SystemParametersInfo(SPI_GETMOUSESPEED, 0, &original_speed_, 0);
}

static bool send_mouse_input_bulk(const MOUSEINPUT* inputs, uint32_t count) {
	auto& logitech = send::Logitech::get_logitech_instance();
	for (uint32_t i = 0; i < count; ++i) {
		if (!logitech.send_mouse_report(inputs[i])) return false;
	}
	return true;
}

struct MouseMapping {
	DWORD down;
	DWORD up;
	DWORD data;
};

static constexpr MouseMapping kMappings[] = {
	{MOUSEEVENTF_LEFTDOWN,   MOUSEEVENTF_LEFTUP,   0},      // LEFT   = 1
	{MOUSEEVENTF_RIGHTDOWN,  MOUSEEVENTF_RIGHTUP,  0},      // RIGHT  = 2
	{MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, 0},      // MIDDLE = 3
	{MOUSEEVENTF_XDOWN,      MOUSEEVENTF_XUP,      0x0001}, // X1     = 4
	{MOUSEEVENTF_XDOWN,      MOUSEEVENTF_XUP,      0x0002}, // X2     = 5
};

static const MouseMapping* lookup(MouseButton button) {
	auto idx = static_cast<uint16_t>(button);
	if (idx < 1 || idx > 5) return nullptr;
	return &kMappings[idx - 1];
}

// 将大数值拆分为多个 HID 兼容的步进报告
template<typename F>
static bool step_reports(int32_t total_dx, int32_t total_dy, int32_t max_step, F&& fill) {
    int32_t steps = (std::max)(
        (std::abs(total_dx) + max_step - 1) / max_step,
        (std::abs(total_dy) + max_step - 1) / max_step);
    if (steps < 1) steps = 1;

    float step_dx = static_cast<float>(total_dx) / steps;
    float step_dy = static_cast<float>(total_dy) / steps;
    float prev_x = 0, prev_y = 0;

    std::vector<MOUSEINPUT> reports;
    reports.reserve(steps);
    for (int32_t i = 1; i <= steps; ++i) {
        float curr_x = step_dx * i;
        float curr_y = step_dy * i;

        MOUSEINPUT mi{};
        fill(mi,
            static_cast<int32_t>(curr_x - prev_x + 0.5f),
            static_cast<int32_t>(curr_y - prev_y + 0.5f));
        reports.push_back(mi);

        prev_x = curr_x;
        prev_y = curr_y;
    }
    return send_mouse_input_bulk(reports.data(), static_cast<uint32_t>(reports.size()));
}

DLLAPI bool WINAPI mouse_down(MouseButton button) {
	auto m = lookup(button);
	if (!m) return false;
	MOUSEINPUT mi{};
	mi.dwFlags = m->down;
	mi.mouseData = m->data;
	return send_mouse_input_bulk(&mi, 1);
}

DLLAPI bool WINAPI mouse_up(MouseButton button) {
	auto m = lookup(button);
	if (!m) return false;
	MOUSEINPUT mi{};
	mi.dwFlags = m->up;
	mi.mouseData = m->data;
	return send_mouse_input_bulk(&mi, 1);
}

DLLAPI bool WINAPI mouse_click(MouseButton button) {
	auto m = lookup(button);
	if (!m) return false;
	MOUSEINPUT inputs[2]{};
	inputs[0].dwFlags = m->down;
	inputs[0].mouseData = m->data;
	inputs[1].dwFlags = m->up;
	inputs[1].mouseData = m->data;
	return send_mouse_input_bulk(inputs, 2);
}

DLLAPI bool WINAPI mouse_move_relative(int32_t dx, int32_t dy) {
	//纠正系数
	float coeff = get_mouse_move_coefficient();
	dx = static_cast<int32_t>(dx * coeff);
	dy = static_cast<int32_t>(dy * coeff);

	return step_reports(dx, dy, 128, [](MOUSEINPUT& mi, int32_t sx, int32_t sy) {
		mi.dx = sx; mi.dy = sy; mi.dwFlags = MOUSEEVENTF_MOVE;
	});
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
	return step_reports(movement, 0, 120, [](MOUSEINPUT& mi, int32_t delta, int32_t) {
		mi.mouseData = static_cast<DWORD>(delta); mi.dwFlags = MOUSEEVENTF_WHEEL;
	});
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
		return;
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
