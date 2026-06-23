// logitech.h
#pragma once
#include "logitech_driver.h"

namespace send {

	class Logitech {
	private:
		LogitechDriver driver_{};
		LogitechDriver::MouseReport mouse_report_{};
		LogitechDriver::KeyboardReport keyboard_report_{};
		std::mutex mouse_mutex_{};
		std::mutex keyboard_mutex_{};

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
		static Logitech& get_logitech_instance();
		bool send_keyboard_report(KeyCode vk, bool down);
		bool send_mouse_report(const MOUSEINPUT& mi);
		void release_all_mouse();
		void release_all_keys();

	};

} // namespace send
