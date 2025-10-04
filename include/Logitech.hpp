//Logitech.hpp
#pragma once
#include <LogitechDriver.hpp>

namespace Send {

	class Logitech {
	private:
		LogitechDriver driver{};
		//LogitechDriver::MouseReport mouse_report{};
		LogitechDriver::KeyboardReport keyboard_report{};// 键盘报告,记录按键快照状态.
		std::mutex mouse_mutex;
		std::mutex keyboard_mutex;
		
		// 禁止拷贝
		Logitech(const Logitech&) = delete;
		Logitech& operator=(const Logitech&) = delete;

		// 支持移动语义
		Logitech(Logitech&&) = delete;
		Logitech& operator=(Logitech&&) = delete;

	private:
		Logitech();
		~Logitech();

	public:
		static Logitech& getLogitechInstance();
		bool send_keyboard_report(const KEYBDINPUT& ki);
		bool send_mouse_report(const MOUSEINPUT& mi);

	};

} // namespace Send

