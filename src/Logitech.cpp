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
		release_all_keys();
		release_all_mouse();
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
			if (mi.mouseData & XBUTTON1) btn.XButton1 = true;
			if (mi.mouseData & XBUTTON2) btn.XButton2 = true;
		}
		if (mi.dwFlags & MOUSEEVENTF_XUP) {
			if (mi.mouseData & XBUTTON1) btn.XButton1 = false;
			if (mi.mouseData & XBUTTON2) btn.XButton2 = false;
		}
	}

	// 发送鼠标报告（支持移动、滚轮、按键等事件）
	bool Logitech::send_mouse_report(const MOUSEINPUT& mi) {
		std::lock_guard lock(mouse_mutex);

		// 处理鼠标移动
		if (mi.dwFlags & MOUSEEVENTF_MOVE) {
			mouse_report.x = mi.dx;
			mouse_report.y = mi.dy;
		}
		else {
			//鼠标移动状态不需要维护，清零
			mouse_report.x = 0;
			mouse_report.y = 0;
		}

		// 处理鼠标滚轮
		if (mi.dwFlags & MOUSEEVENTF_WHEEL) {
			mouse_report.wheel = (static_cast<int32_t>(mi.mouseData) > 0) ? 1 : -1;
		}

		// 按键
		if (mi.dwFlags & (MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP |
			MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP |
			MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP |
			MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP))
		{
			update_mouse_button(mouse_report.button, mi);
		}

		return driver.report_mouse(mouse_report);
	}

	// 发送键盘输入事件（支持修饰键状态更新）
	bool Logitech::send_keyboard_report(const KEYBDINPUT& ki) {
		std::lock_guard lock(keyboard_mutex);

		bool keydown = !(ki.dwFlags & KEYEVENTF_KEYUP);

		switch (ki.wVk) {
		case VK_CONTROL:  keyboard_report.modifiers.RCtrl = keydown; break;
		case VK_LCONTROL: keyboard_report.modifiers.LCtrl = keydown; break;
		case VK_RCONTROL: keyboard_report.modifiers.RCtrl = keydown; break;
		case VK_SHIFT:    keyboard_report.modifiers.RShift = keydown; break;
		case VK_LSHIFT:   keyboard_report.modifiers.LShift = keydown; break;
		case VK_RSHIFT:   keyboard_report.modifiers.RShift = keydown; break;
		case VK_MENU:     keyboard_report.modifiers.RAlt = keydown; break;
		case VK_LMENU:    keyboard_report.modifiers.LAlt = keydown; break;
		case VK_RMENU:    keyboard_report.modifiers.RAlt = keydown; break;
		case VK_LWIN:     keyboard_report.modifiers.LGui = keydown; break;
		case VK_RWIN:     keyboard_report.modifiers.RGui = keydown; break;

		default:
			// 普通按键处理
			uint8_t usage = Usb::keyboard_vk_to_usage((uint8_t)ki.wVk);

			if (keydown) {
				// 按下：检查是否已经存在，避免重复
				bool already_pressed = false;
				for (int i = 0; i < 6; i++) {
					if (keyboard_report.keys[i] == usage) {
						already_pressed = true;
						break;
					}
				}

				if (already_pressed) {
					// 已经按下，不再处理
					break;
				}

				// 填入空位
				bool inserted = false;
				for (int i = 0; i < 6; i++) {
					if (keyboard_report.keys[i] == 0) {
						keyboard_report.keys[i] = usage;
						inserted = true;
						break;
					}
				}

				if (!inserted) {
					printf("按键数量超过6个限制!\n");
					OutputDebugStringA("按键数量超过6个限制!\n");
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

	//释放所有鼠标按键：清空鼠标 HID 报告并提交
	void Logitech::release_all_mouse() {
		std::lock_guard lock(mouse_mutex);

		// 清空所有按钮状态
		mouse_report.button_byte = 0;

		// 清空相对位移与滚轮
		mouse_report.x = 0;
		mouse_report.y = 0;
		mouse_report.wheel = 0;

		// 发送全 0 鼠标报告（所有按钮抬起）
		driver.report_mouse(mouse_report);
	}

	//释放所有按下的键：清空 HID 报告并提交
	void Logitech::release_all_keys() {
		std::lock_guard lock(keyboard_mutex);

		memset(&keyboard_report.modifiers, 0, sizeof(keyboard_report.modifiers));
		memset(keyboard_report.keys, 0, sizeof(keyboard_report.keys));

		// 发送全 0 HID 报告告诉系统“没有按键按下”
		driver.report_keyboard(keyboard_report);
	}



}
