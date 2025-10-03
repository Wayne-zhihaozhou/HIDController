//Logitech.hpp
#pragma once
#include "SendTypes.hpp"
#include "LogitechDriver.hpp"

namespace Send::Internal {

	class Logitech : public Send::Internal::Base {
	private:
		LogitechDriver driver{};
		std::mutex mouse_mutex;
		LogitechDriver::KeyboardReport keyboard_report{};
		std::mutex keyboard_mutex;

		struct MouseReport {
			union {
				LogitechDriver::MouseButton button;
				uint8_t button_byte;
			};
			uint8_t x;
			uint8_t y;
			uint8_t wheel;
			uint8_t unknown_T;

		//private:
			//void assert_size() { static_assert(sizeof(MouseReport) == 5); }
		};


		//bool send_mouse_report(const MOUSEINPUT& mi);
		
		

	public:
		Logitech();
		Send::Error create();
		void destroy();

		//virtual uint32_t send_mouse_input(const INPUT inputs[], uint32_t n);
		//virtual bool send_mouse_input(const MOUSEINPUT& mi);
		bool send_keyboard_input(const KEYBDINPUT& ki);

		bool send_mouse_report(const MOUSEINPUT& mi);




	};

} // namespace Send::Internal

