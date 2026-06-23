// logitech.cpp
#include "pch.h"
#include "logitech.h"
#include "keyboard_map.h"

namespace send {

	// 构造函数：RAII初始化驱动
	Logitech::Logitech() {
		driver_.create();
	}

	// 析构函数：RAII释放驱动
	Logitech::~Logitech() {
		release_all_keys();
		release_all_mouse();
		driver_.destroy();
	}

	// 获取单例
	Logitech& Logitech::get_logitech_instance() {
		static Logitech instance; // 延迟初始化，线程安全
		return instance;
	}

// 辅助函数
void update_mouse_button(LogitechDriver::MouseButton& btn, const MOUSEINPUT& mi) {
		// 左键
		if (mi.dwFlags & MOUSEEVENTF_LEFTDOWN) btn.LButton_ = true;
		if (mi.dwFlags & MOUSEEVENTF_LEFTUP)   btn.LButton_ = false;

		// 右键
		if (mi.dwFlags & MOUSEEVENTF_RIGHTDOWN) btn.RButton_ = true;
		if (mi.dwFlags & MOUSEEVENTF_RIGHTUP)   btn.RButton_ = false;

		// 中键
		if (mi.dwFlags & MOUSEEVENTF_MIDDLEDOWN) btn.MButton_ = true;
		if (mi.dwFlags & MOUSEEVENTF_MIDDLEUP)   btn.MButton_ = false;

		// X 按钮
		if (mi.dwFlags & MOUSEEVENTF_XDOWN) {
			if (mi.mouseData & XBUTTON1) btn.XButton1_ = true;
			if (mi.mouseData & XBUTTON2) btn.XButton2_ = true;
		}
		if (mi.dwFlags & MOUSEEVENTF_XUP) {
			if (mi.mouseData & XBUTTON1) btn.XButton1_ = false;
			if (mi.mouseData & XBUTTON2) btn.XButton2_ = false;
		}
	}

	// 发送鼠标报告（支持移动、滚轮、按键等事件）
	bool Logitech::send_mouse_report(const MOUSEINPUT& mi) {
		std::lock_guard lock(mouse_mutex_);

		// 处理鼠标移动
		if (mi.dwFlags & MOUSEEVENTF_MOVE) {
			mouse_report_.x_ = mi.dx;
			mouse_report_.y_ = mi.dy;
		}
		else {
			//鼠标移动状态不需要维护，清零
			mouse_report_.x_ = 0;
			mouse_report_.y_ = 0;
		}

		// 处理鼠标滚轮
		if (mi.dwFlags & MOUSEEVENTF_WHEEL) {
			mouse_report_.wheel_ = (static_cast<int32_t>(mi.mouseData) > 0) ? 1 : -1;
		}

		// 按键
		if (mi.dwFlags & (MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP |
			MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP |
			MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP |
			MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP))
		{
			update_mouse_button(mouse_report_.button_, mi);
		}

		return driver_.report_mouse(mouse_report_);
	}

	// 发送键盘输入事件（支持修饰键状态更新）
	bool Logitech::send_keyboard_report(const KEYBDINPUT& ki) {
		std::lock_guard lock(keyboard_mutex_);

		bool keydown = !(ki.dwFlags & KEYEVENTF_KEYUP);

		switch (static_cast<KeyCode>(ki.wVk)) {
		case KeyCode::CTRL:  keyboard_report_.modifiers_.RCtrl_ = keydown; break;
		case KeyCode::LCTRL: keyboard_report_.modifiers_.LCtrl_ = keydown; break;
		case KeyCode::RCTRL: keyboard_report_.modifiers_.RCtrl_ = keydown; break;
		case KeyCode::SHIFT:    keyboard_report_.modifiers_.RShift_ = keydown; break;
		case KeyCode::LSHIFT:   keyboard_report_.modifiers_.LShift_ = keydown; break;
		case KeyCode::RSHIFT:   keyboard_report_.modifiers_.RShift_ = keydown; break;
		case KeyCode::ALT:  keyboard_report_.modifiers_.RAlt_ = keydown; break;
		case KeyCode::LALT:    keyboard_report_.modifiers_.LAlt_ = keydown; break;
		case KeyCode::RALT:    keyboard_report_.modifiers_.RAlt_ = keydown; break;
		case KeyCode::LWIN:     keyboard_report_.modifiers_.LGui_ = keydown; break;
		case KeyCode::RWIN:     keyboard_report_.modifiers_.RGui_ = keydown; break;

		default:
			// 普通按键处理
			uint8_t usage = usb::keyboard_vk_to_usage(static_cast<KeyCode>(ki.wVk));

			if (keydown) {
				// 按下：检查是否已经存在，避免重复
				bool already_pressed = false;
				for (int i = 0; i < 6; i++) {
					if (keyboard_report_.keys_[i] == usage) {
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
					if (keyboard_report_.keys_[i] == 0) {
						keyboard_report_.keys_[i] = usage;
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
					if (keyboard_report_.keys_[i] == usage) {
						keyboard_report_.keys_[i] = 0;
						break;
					}
				}
			}
			break;
		}

		// 提交键盘报告
		return driver_.report_keyboard(keyboard_report_);
	}

	//释放所有鼠标按键：清空鼠标 HID 报告并提交
	void Logitech::release_all_mouse() {
		std::lock_guard lock(mouse_mutex_);

		// 清空所有按钮状态
		mouse_report_.button_byte_ = 0;

		// 清空相对位移与滚轮
		mouse_report_.x_ = 0;
		mouse_report_.y_ = 0;
		mouse_report_.wheel_ = 0;

		// 发送全 0 鼠标报告（所有按钮抬起）
		driver_.report_mouse(mouse_report_);
	}

	//释放所有按下的键：清空 HID 报告并提交
	void Logitech::release_all_keys() {
		std::lock_guard lock(keyboard_mutex_);

		memset(&keyboard_report_.modifiers_, 0, sizeof(keyboard_report_.modifiers_));
		memset(keyboard_report_.keys_, 0, sizeof(keyboard_report_.keys_));

		// 发送全 0 HID 报告告诉系统"没有按键按下"
		driver_.report_keyboard(keyboard_report_);
	}



}
