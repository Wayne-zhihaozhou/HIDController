//Logitech.cpp
#include"pch.h"
#include "Logitech.hpp"
#include "base.hpp"
#include "SendTypes.hpp"
#include "KeyboardMap.hpp"



namespace Send::Internal {

	// 构造函数
	Logitech::Logitech() = default;

	// 初始化 Logitech 驱动
	Send::Error Logitech::create() {
		return driver.create();
	}

	// 销毁 Logitech 驱动
	void Logitech::destroy() {
		driver.destroy();
	}

	// 发送一组鼠标输入事件
	uint32_t Logitech::send_mouse_input(const INPUT inputs[], uint32_t n) {
		uint32_t count = 0;
		for (uint32_t i = 0; i < n; ++i) {
			if (inputs[i].type == INPUT_MOUSE) {
				if (send_mouse_input(inputs[i].mi)) {
					++count;
				}
			}
		}
		return count;
	}

	// 发送单个鼠标输入事件
	bool Logitech::send_mouse_input(const MOUSEINPUT& mi) {
		return send_mouse_report<MouseReport>(mi);
	}

	// 发送键盘输入事件（支持修饰键状态更新）
	bool Logitech::send_keyboard_input(const KEYBDINPUT& ki) {
		std::lock_guard lock(keyboard_mutex);

		bool keydown = !(ki.dwFlags & KEYEVENTF_KEYUP);

		// 处理修饰键
		switch (ki.wVk) {
		case VK_LCONTROL: keyboard_report.modifiers.LCtrl = keydown; break;
		case VK_RCONTROL: keyboard_report.modifiers.RCtrl = keydown; break;
		case VK_LSHIFT:   keyboard_report.modifiers.LShift = keydown; break;
		case VK_RSHIFT:   keyboard_report.modifiers.RShift = keydown; break;
		case VK_LMENU:    keyboard_report.modifiers.LAlt = keydown; break;
		case VK_RMENU:    keyboard_report.modifiers.RAlt = keydown; break;
		case VK_LWIN:     keyboard_report.modifiers.LGui = keydown; break;
		case VK_RWIN:     keyboard_report.modifiers.RGui = keydown; break;

		default:
			// 普通按键处理
			uint8_t usage = Usb::keyboard_vk_to_usage((uint8_t)ki.wVk);
			if (keydown) {
				// 按下：填入空位
				for (int i = 0; i < 6; i++) {
					if (keyboard_report.keys[i] == 0) {
						keyboard_report.keys[i] = usage;
						break;
					}
				}
			}
			else {
				// 抬起：清除对应按键
				for (int i = 0; i < 6; i++) {
					if (keyboard_report.keys[i] == usage) {
						keyboard_report.keys[i] = 0;
						break;
					}
				}
			}
			break;
		}

		// 提交键盘报告
		return driver.report_keyboard(keyboard_report);
	}

	// 发送鼠标报告（支持移动、滚轮、按键等事件）
	template <class ReportType>
	bool Logitech::send_mouse_report(const MOUSEINPUT& mi) {
		std::lock_guard lock(mouse_mutex);

		ReportType mouse_report{};

		// 处理鼠标移动
		if (mi.dwFlags & MOUSEEVENTF_MOVE) {
			POINT move{ mi.dx, mi.dy };
			if (mi.dwFlags & MOUSEEVENTF_ABSOLUTE) {
				// 坐标转换：绝对 -> 屏幕坐标 -> 相对坐标
				if (mi.dwFlags & MOUSEEVENTF_VIRTUALDESK)
					mouse_virtual_desk_absolute_to_screen(move);
				else
					mouse_absolute_to_screen(move);
				mouse_screen_to_relative(move);
			}

			// 分段处理大移动量，避免超出坐标类型范围
			static_assert(std::is_same_v<decltype(mouse_report.x), decltype(mouse_report.y)>);
			using CoordinatesType = decltype(mouse_report.x);
			constexpr auto maxValue = max_value<CoordinatesType>();

			while (abs(move.x) > maxValue || abs(move.y) > maxValue) {
				if (abs(move.x) > maxValue) {
					mouse_report.x = move.x > 0 ? maxValue : -maxValue;
					move.x -= mouse_report.x;
				}
				else {
					mouse_report.x = 0;
				}

				if (abs(move.y) > maxValue) {
					mouse_report.y = move.y > 0 ? maxValue : -maxValue;
					move.y -= mouse_report.y;
				}
				else {
					mouse_report.y = 0;
				}

				// 分段发送移动报告
				driver.report_mouse(mouse_report, compensate_switch = -compensate_switch);
			}

			mouse_report.x = (CoordinatesType)move.x;
			mouse_report.y = (CoordinatesType)move.y;
		}
		else {
			mouse_report.x = 0;
			mouse_report.y = 0;
		}

		// 处理滚轮事件
		if (mi.dwFlags & MOUSEEVENTF_WHEEL) {
			mouse_report.wheel = std::bit_cast<int32_t>(mi.mouseData) > 0 ? 1 : -1;
		}

		// 处理左右中按键
#define CODE_GENERATE(down, up, member)  \
            if (mi.dwFlags & down || mi.dwFlags & up)  \
                mouse_report.button.##member = mi.dwFlags & down;

		CODE_GENERATE(MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, LButton)
			CODE_GENERATE(MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, RButton)
			CODE_GENERATE(MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, MButton)
#undef CODE_GENERATE

			// 处理 X 按钮
			if (mi.dwFlags & (MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP)) {
				bool down = mi.dwFlags & MOUSEEVENTF_XDOWN;
				switch (mi.mouseData) {
				case XBUTTON1: mouse_report.button.XButton1 = down; break;
				case XBUTTON2: mouse_report.button.XButton2 = down; break;
				}
			}

		// 提交鼠标报告
		return driver.report_mouse(mouse_report, compensate_switch = -compensate_switch);
	}

}
