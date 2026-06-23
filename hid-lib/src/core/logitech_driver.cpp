#include "pch.h"
#include "logitech_driver.h"

namespace send {

std::wstring scan_devices(std::function<bool(std::wstring_view)> predicate) {
	std::wstring result{};
	HANDLE dir_handle;
	OBJECT_ATTRIBUTES obj_attr;
	UNICODE_STRING object_name;
	RtlInitUnicodeString(&object_name, LR"(\GLOBAL??)");
	InitializeObjectAttributes(&obj_attr, &object_name, 0, NULL, NULL);
	if (NT_SUCCESS(NtOpenDirectoryObject(&dir_handle, kDirectoryQuery, &obj_attr))) {
		union { std::uint8_t buf[2048]; OBJECT_DIRECTORY_INFORMATION info[1]; };
		ULONG context;
		NTSTATUS status = NtQueryDirectoryObject(dir_handle, buf, sizeof buf, false, true, &context, NULL);
		while (NT_SUCCESS(status)) {
			bool found = false;
			for (ULONG i = 0; info[i].Name.Buffer; i++) {
				std::wstring_view sv{ info[i].Name.Buffer, info[i].Name.Length / sizeof(wchar_t) };
				if (predicate(sv)) { result = LR"(\??\)" + std::wstring(sv); found = true; break; }
			}
			if (found || status != STATUS_MORE_ENTRIES) break;
			status = NtQueryDirectoryObject(dir_handle, buf, sizeof buf, false, false, &context, NULL);
		}
		CloseHandle(dir_handle);
	}
	return result;
}

static bool ends_with(const std::wstring& str, const std::wstring& suffix) {
	if (str.length() >= suffix.length()) return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
	return false;
}

std::wstring LogitechDriver::find_device() {
	return scan_devices([](std::wstring_view sv) {
		std::wstring ws(sv);
		return ((ws.find(L"ROOT#SYSTEM#") == 0) || (ws.find(L"Root#SYSTEM#") == 0)) &&
			(ends_with(ws, L"#{1abc05c0-c378-41b9-9cef-df1aba82b015}") ||
				ends_with(ws, L"#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}") ||
				ends_with(ws, L"#{dfbedcdb-2148-416d-9e4d-cecc2424128c}") ||
				ends_with(ws, L"#{5bada891-842b-4296-a496-68ae931aa16c}"));
		});
}

bool LogitechDriver::create() {
	std::wstring device_name = find_device();
	if (device_name.empty()) return false;
	device_ = CreateFileW(device_name.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (device_ == INVALID_HANDLE_VALUE) return false;
	return true;
}

void LogitechDriver::destroy() {
	if (device_ != INVALID_HANDLE_VALUE) {
		CloseHandle(device_);
		device_ = INVALID_HANDLE_VALUE;
	}
}

bool LogitechDriver::report_mouse(const MouseReport& report) const {
	return report_ioctl(0x2A2010, report);
}

bool LogitechDriver::report_keyboard(const KeyboardReport& report) const {
	return report_ioctl(0x2A200C, report);
}

}
