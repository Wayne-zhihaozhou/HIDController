//LogitechDriver.hpp
#pragma once
#include "SendTypes.hpp"


namespace Send::Internal {
	class LogitechDriver {
		HANDLE device;

	public:

		struct MouseButton {
			bool LButton : 1;
			bool RButton : 1;
			bool MButton : 1;
			bool XButton1 : 1;
			bool XButton2 : 1;
			bool unknown : 3;
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


		//可以取消模板,使用普通函数
		template <class ReportType>
		bool report_mouse(ReportType report) const {
			constexpr DWORD IOCTL_BUSENUM_PLAY_MOUSEMOVE = 0x2A2010;
			DWORD bytes_returned;

			return DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &report, sizeof(ReportType), nullptr, 0, &bytes_returned, nullptr);
		}

		bool report_keyboard(KeyboardReport report) const;


	private:
		//HANDLE device{ INVALID_HANDLE_VALUE };删除?

		std::wstring find_device();
	};
}




