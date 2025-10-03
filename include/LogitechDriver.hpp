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

		struct KeyboardReport {
			union {
				KeyboardModifiers modifiers;
				uint8_t modifiers_byte;
			};
			uint8_t reserved;
			uint8_t keys[6];
		};

	public:
		Send::Error create();
		void destroy();


		////可以取消模板,使用普通函数
		//template <class ReportType>
		//bool report_mouse(ReportType report) const {
		//	constexpr DWORD IOCTL_BUSENUM_PLAY_MOUSEMOVE = 0x2A2010;
		//	DWORD bytes_returned;

		//	return DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &report, sizeof(ReportType), nullptr, 0, &bytes_returned, nullptr);
		//}

		bool report_mouse(MouseReport& report) const;
		bool report_keyboard(KeyboardReport report) const;


	private:
		//HANDLE device{ INVALID_HANDLE_VALUE };删除?

		std::wstring find_device();

	};
}




