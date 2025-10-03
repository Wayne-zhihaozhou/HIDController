//Logitech.cpp
#include"pch.h"
#include "Logitech.hpp"
#include "base.hpp"
#include "SendTypes.hpp"
#include "KeyboardMap.hpp"

#include <algorithm>


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
	//uint32_t Logitech::send_mouse_input(const INPUT inputs[], uint32_t n) {
	//	uint32_t count = 0;
	//	for (uint32_t i = 0; i < n; ++i) {
	//		if (inputs[i].type == INPUT_MOUSE) {
	//			if (send_mouse_report(inputs[i].mi)) {
	//				++count;
	//			}
	//		}
	//	}
	//	return count;
	//}

	// 发送单个鼠标输入事件
	//bool Logitech::send_mouse_input(const MOUSEINPUT& mi) {
	//	return send_mouse_report(mi);
	//}

	// 发送键盘输入事件（支持修饰键状态更新）
	bool Logitech::send_keyboard_report(const KEYBDINPUT& ki) {
		std::lock_guard lock(keyboard_mutex);

		bool keydown = !(ki.dwFlags & KEYEVENTF_KEYUP);

		// 处理修饰键
		//switch (ki.wVk) {
		//case VK_LCONTROL: keyboard_report.modifiers.LCtrl = keydown; break;
		//case VK_RCONTROL: keyboard_report.modifiers.RCtrl = keydown; break;
		//case VK_LSHIFT:   keyboard_report.modifiers.LShift = keydown; break;
		//case VK_RSHIFT:   keyboard_report.modifiers.RShift = keydown; break;
		//case VK_LMENU:    keyboard_report.modifiers.LAlt = keydown; break;
		//case VK_RMENU:    keyboard_report.modifiers.RAlt = keydown; break;
		//case VK_LWIN:     keyboard_report.modifiers.LGui = keydown; break;
		//case VK_RWIN:     keyboard_report.modifiers.RGui = keydown; break;

		switch (ki.wVk) {
			// ✅ Ctrl
		case VK_CONTROL:  keyboard_report.modifiers.RCtrl = keydown; break;
		case VK_LCONTROL: keyboard_report.modifiers.LCtrl = keydown; break;
		case VK_RCONTROL: keyboard_report.modifiers.RCtrl = keydown; break;

			// ✅ Shift
		case VK_SHIFT:	  keyboard_report.modifiers.RShift = keydown; break;
		case VK_LSHIFT:   keyboard_report.modifiers.LShift = keydown; break;
		case VK_RSHIFT:   keyboard_report.modifiers.RShift = keydown; break;

			// ✅ Alt
		case VK_MENU:	  keyboard_report.modifiers.RAlt = keydown; break;
		case VK_LMENU:    keyboard_report.modifiers.LAlt = keydown; break;
		case VK_RMENU:    keyboard_report.modifiers.RAlt = keydown; break;

			// ✅ Win
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

	// 发送鼠标报告（支持移动、滚轮、按键等事件）//////////////////////////////
	void set_button(LogitechDriver::MouseButton& btn, uint32_t dwFlags, uint32_t downFlag, uint32_t upFlag) {
		if (dwFlags & (downFlag | upFlag))
			btn = static_cast<LogitechDriver::MouseButton>((dwFlags & downFlag) != 0);
	}

	bool Logitech::send_mouse_report(const MOUSEINPUT& mi) {
		std::lock_guard lock(mouse_mutex);

		MouseReport mouse_report{};
		mouse_report.x = 0;
		mouse_report.y = 0;
		mouse_report.wheel = 0;
		mouse_report.button_byte = 0;

		// 处理鼠标移动
		if (mi.dwFlags & MOUSEEVENTF_MOVE) {
			POINT move{ mi.dx, mi.dy };

			if (mi.dwFlags & MOUSEEVENTF_ABSOLUTE) {
				if (mi.dwFlags & MOUSEEVENTF_VIRTUALDESK)
					mouse_virtual_desk_absolute_to_screen(move);
				else
					mouse_absolute_to_screen(move);
				mouse_screen_to_relative(move);
			}

			mouse_report.x = move.x;
			mouse_report.y = move.y;
		}

		// 处理滚轮
		if (mi.dwFlags & MOUSEEVENTF_WHEEL) {
			mouse_report.wheel = (static_cast<int32_t>(mi.mouseData) > 0) ? 1 : -1;
		}

		// 处理鼠标按键
		auto set_button = [](LogitechDriver::MouseButton& btn, uint32_t dwFlags, uint32_t downFlag, uint32_t upFlag) {
			if (dwFlags & (downFlag | upFlag))
				btn = static_cast<LogitechDriver::MouseButton>((dwFlags & downFlag) != 0);
			};

		set_button(mouse_report.button, mi.dwFlags, MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP);
		set_button(mouse_report.button, mi.dwFlags, MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP);
		set_button(mouse_report.button, mi.dwFlags, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP);

		//X 按钮
		if (mi.dwFlags & (MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP)) {
			bool down = mi.dwFlags & MOUSEEVENTF_XDOWN;
			if (mi.mouseData == XBUTTON1) mouse_report.button.XButton1 = down;
			if (mi.mouseData == XBUTTON2) mouse_report.button.XButton2 = down;
		}

		return driver.report_mouse(mouse_report);
	}


}
