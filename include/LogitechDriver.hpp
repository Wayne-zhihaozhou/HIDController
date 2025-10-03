//LogitechDriver.hpp
#pragma once
#include "SendTypes.hpp"


namespace Send::Internal {
	class LogitechDriver {
	public:
		HANDLE device;

		struct MouseButton {
			bool LButton : 1;
			bool RButton : 1;
			bool MButton : 1;
			bool XButton1 : 1;
			bool XButton2 : 1;
			bool unknown : 3;
		};

		struct KeyboardModifiers {
			bool LCtrl : 1;
			bool LShift : 1;
			bool LAlt : 1;
			bool LGui : 1;
			bool RCtrl : 1;
			bool RShift : 1;
			bool RAlt : 1;
			bool RGui : 1;
		};

		struct MouseReport {
			union {
				MouseButton button;
				uint8_t button_byte;
			};
			uint8_t x;
			uint8_t y;
			uint8_t wheel;
			uint8_t unknown_T;

		private:
			void assert_size() { static_assert(sizeof(MouseReport) == 5); }
		};

		struct KeyboardReport {
			union {
				KeyboardModifiers modifiers;
				uint8_t modifiers_byte;
			};
			uint8_t reserved;
			uint8_t keys[6];
		};

	private:
		std::wstring find_device();

	public:
		bool create();
		void destroy();
		bool report_mouse(const MouseReport& report) const;
		bool report_keyboard(const KeyboardReport& report) const;
		

	};
}




