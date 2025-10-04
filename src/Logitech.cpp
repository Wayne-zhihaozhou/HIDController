//Logitech.cpp
#include"pch.h"
#include "Logitech.hpp"
#include "KeyboardMap.hpp"


namespace Send {

	// 构造函数：RAII初始化驱动
	Logitech::Logitech() {
		driver.create();
	}

	// 析构函数：RAII释放驱动
	Logitech::~Logitech() {
		driver.destroy();
	}

	// 获取单例
	Logitech& Logitech::getLogitechInstance() {
		static Logitech instance; // 延迟初始化，线程安全
		return instance;
	}

	//辅助函数
	void update_mouse_button(LogitechDriver::MouseButton& btn, const MOUSEINPUT& mi) {
		// 左键
		if (mi.dwFlags & MOUSEEVENTF_LEFTDOWN) btn.LButton = true;
		if (mi.dwFlags & MOUSEEVENTF_LEFTUP)   btn.LButton = false;

		// 右键
		if (mi.dwFlags & MOUSEEVENTF_RIGHTDOWN) btn.RButton = true;
		if (mi.dwFlags & MOUSEEVENTF_RIGHTUP)   btn.RButton = false;

		// 中键
		if (mi.dwFlags & MOUSEEVENTF_MIDDLEDOWN) btn.MButton = true;
		if (mi.dwFlags & MOUSEEVENTF_MIDDLEUP)   btn.MButton = false;

		// X 按钮
		if (mi.dwFlags & MOUSEEVENTF_XDOWN) {
			if (mi.mouseData == XBUTTON1) btn.XButton1 = true;
			if (mi.mouseData == XBUTTON2) btn.XButton2 = true;
		}
		if (mi.dwFlags & MOUSEEVENTF_XUP) {
			if (mi.mouseData == XBUTTON1) btn.XButton1 = false;
			if (mi.mouseData == XBUTTON2) btn.XButton2 = false;
		}
	}

	// 发送鼠标报告（支持移动、滚轮、按键等事件）
	bool Logitech::send_mouse_report(const MOUSEINPUT& mi) {
		std::lock_guard lock(mouse_mutex);

		//初始化
		Send::LogitechDriver::MouseReport mouse_report{};
		mouse_report.x = 0;
		mouse_report.y = 0;
		mouse_report.wheel = 0;
		mouse_report.button_byte = 0;

		// 处理鼠标移动
		if (mi.dwFlags & MOUSEEVENTF_MOVE) {
			mouse_report.x = mi.dx;
			mouse_report.y = mi.dy;
		}

		// 处理鼠标滚轮
		if (mi.dwFlags & MOUSEEVENTF_WHEEL) {
			mouse_report.wheel = (static_cast<int32_t>(mi.mouseData) > 0) ? 1 : -1;
		}

		// 处理鼠标按键
		update_mouse_button(mouse_report.button, mi);

		return driver.report_mouse(mouse_report);
	}

	// 发送键盘输入事件（支持修饰键状态更新）
	bool Logitech::send_keyboard_report(const KEYBDINPUT& ki) {
		std::lock_guard lock(keyboard_mutex);
		/*LogitechDriver::KeyboardReport keyboard_report{};*/
		bool keydown = !(ki.dwFlags & KEYEVENTF_KEYUP);

		switch (ki.wVk) {
		case VK_CONTROL:  keyboard_report.modifiers.RCtrl = keydown; break;
		case VK_LCONTROL: keyboard_report.modifiers.LCtrl = keydown; break;
		case VK_RCONTROL: keyboard_report.modifiers.RCtrl = keydown; break;
		case VK_SHIFT:	  keyboard_report.modifiers.RShift = keydown; break;
		case VK_LSHIFT:   keyboard_report.modifiers.LShift = keydown; break;
		case VK_RSHIFT:   keyboard_report.modifiers.RShift = keydown; break;
		case VK_MENU:	  keyboard_report.modifiers.RAlt = keydown; break;
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

}
