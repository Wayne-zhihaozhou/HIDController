//LogitechDriver.hpp
#pragma once
#include <windows.h>
#include <string>
#include <mutex>
#include <bit>
#include <cstdint>
#include <cmath>

#include "base.hpp"
#include "SendTypes.hpp"


namespace Send::Type::Internal {
	class LogitechDriver {
		HANDLE device;
		bool has_acceleration;//©ии╬ЁЩ?

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

		template <class ReportType>
		bool report_mouse(ReportType report, int8_t compensate_switch) const {
			constexpr DWORD IOCTL_BUSENUM_PLAY_MOUSEMOVE = 0x2A2010;
			DWORD bytes_returned;

			//if (has_acceleration && (report.x || report.y)) {
			//    ReportType tmp = report;
			//    tmp.x = tmp.y = compensate_switch;
			//    DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &tmp, sizeof(ReportType), nullptr, 0, &bytes_returned, nullptr);

			//    report.x = compensate_lgs_acceleration(report.x);
			//    report.y = compensate_lgs_acceleration(report.y);
			//}

			return DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &report, sizeof(ReportType), nullptr, 0, &bytes_returned, nullptr);
		}

		bool report_keyboard(KeyboardReport report) const;


	private:
		//HANDLE device{ INVALID_HANDLE_VALUE };и╬ЁЩ?

		std::wstring find_device();
	};
}




