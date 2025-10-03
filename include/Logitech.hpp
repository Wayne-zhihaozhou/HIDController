//Logitech.hpp
#pragma once
#include "SendTypes.hpp"
#include "LogitechDriver.hpp"

namespace Send::Internal {

	class Logitech : public Send::Internal::Base {
	private:
		LogitechDriver driver{};
		std::mutex mouse_mutex;
		std::mutex keyboard_mutex;
	public:
		Logitech();
		Send::Error create();
		void destroy();
		bool send_keyboard_report(const KEYBDINPUT& ki);
		bool send_mouse_report(const MOUSEINPUT& mi);

	};

} // namespace Send::Internal

