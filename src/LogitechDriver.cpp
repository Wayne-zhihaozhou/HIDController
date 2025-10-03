// LogitechDriver.cpp
#include"pch.h"
#include"LogitechDriver.hpp"
#include "Logitech.hpp"



namespace Send::Internal {

	// 遍历系统设备目录，查找满足条件的设备路径
	std::wstring find_device(std::function<bool(std::wstring_view name)> p) {
		std::wstring result{};
		HANDLE dir_handle;

		OBJECT_ATTRIBUTES obj_attr;
		UNICODE_STRING obj_name;
		RtlInitUnicodeString(&obj_name, LR"(\GLOBAL??)");
		InitializeObjectAttributes(&obj_attr, &obj_name, 0, NULL, NULL);

		if (NT_SUCCESS(NtOpenDirectoryObject(&dir_handle, DIRECTORY_QUERY, &obj_attr))) {
			union {
				std::uint8_t buf[2048];
				OBJECT_DIRECTORY_INFORMATION info[1];
			};
			ULONG context;

			NTSTATUS status = NtQueryDirectoryObject(dir_handle, buf, sizeof buf, false, true, &context, NULL);
			while (NT_SUCCESS(status)) {
				bool found = false;
				for (ULONG i = 0; info[i].Name.Buffer; i++) {
					std::wstring_view sv{ info[i].Name.Buffer, info[i].Name.Length / sizeof(wchar_t) };
					if (p(sv)) {
						result = LR"(\??\)" + std::wstring(sv);
						found = true;
						break;
					}
				}
				if (found || status != STATUS_MORE_ENTRIES)
					break;
				status = NtQueryDirectoryObject(dir_handle, buf, sizeof buf, false, false, &context, NULL);
			}

			CloseHandle(dir_handle);
		}

		return result;
	}

	// 查找匹配的 Logitech 设备路径
	std::wstring LogitechDriver::find_device() {
		// 根据设备名规则过滤设备对象
		return Internal::find_device([](std::wstring_view sv) {
			using namespace std::literals;
			return (sv.starts_with(L"ROOT#SYSTEM#"sv) || sv.starts_with(L"Root#SYSTEM#"sv)) &&
				(sv.ends_with(L"#{1abc05c0-c378-41b9-9cef-df1aba82b015}"sv) ||
					sv.ends_with(L"#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}"sv) ||
					sv.ends_with(L"#{dfbedcdb-2148-416d-9e4d-cecc2424128c}"sv) ||
					sv.ends_with(L"#{5bada891-842b-4296-a496-68ae931aa16c}"sv));
			});
	}

	// 创建 Logitech 驱动设备连接
	bool LogitechDriver::create() {
		// 查找匹配的设备路径
		std::wstring device_name = find_device();
		if (device_name.empty()) {
			return false;
		}


		// 打开设备句柄
		device = CreateFileW(
			device_name.c_str(),
			GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL
		);

		if (device == INVALID_HANDLE_VALUE) {
			return false;
		}

		return true;
	}

	// 销毁 Logitech 驱动设备连接
	void LogitechDriver::destroy() {
		// 关闭设备句柄
		if (device != INVALID_HANDLE_VALUE) {
			CloseHandle(device);
			device = INVALID_HANDLE_VALUE;
		}
	}

	// 向设备发送鼠标报告数据
	bool LogitechDriver::report_mouse(const MouseReport& report) const {
		constexpr DWORD IOCTL_BUSENUM_PLAY_MOUSEMOVE = 0x2A2010;
		DWORD bytes_returned;

		return DeviceIoControl(
			device,
			IOCTL_BUSENUM_PLAY_MOUSEMOVE,
			const_cast<MouseReport*>(&report),
			sizeof(MouseReport),
			nullptr,
			0,
			&bytes_returned,
			nullptr
		);
	}


	// 向设备发送键盘报告数据
	bool LogitechDriver::report_keyboard(const KeyboardReport& report) const {
		DWORD bytes_returned;
		// 通过 IOCTL 向驱动发送键盘数据
		return DeviceIoControl(
			device,
			0x2A200C,
			const_cast<KeyboardReport*>(&report),
			sizeof(KeyboardReport),
			nullptr,
			0,
			&bytes_returned,
			nullptr
		);
	}

}
