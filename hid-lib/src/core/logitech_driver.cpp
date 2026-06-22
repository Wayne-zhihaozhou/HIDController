#include "pch.h"
#include "logitech_driver.h"
#include <tlhelp32.h>

namespace send {

std::wstring find_executable_in_path(const std::wstring& exe_name = L"LCore.exe") {
	wchar_t* path_env = nullptr;
	size_t len = 0;
	_wdupenv_s(&path_env, &len, L"PATH");
	if (!path_env) return L"";
	std::wstring path_str(path_env);
	free(path_env);
	std::wstringstream ss(path_str);
	std::wstring folder;
	while (std::getline(ss, folder, L';')) {
		std::filesystem::path full_path = folder;
		full_path /= exe_name;
		if (std::filesystem::exists(full_path)) return full_path.wstring();
	}
	return L"";
}

bool start_process(const std::wstring& process_path) {
	STARTUPINFOW si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof(si);
	if (!CreateProcessW(process_path.c_str(), nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) return false;
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}

bool is_process_running(const std::wstring& process_name) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) return false;
	PROCESSENTRY32W entry{ sizeof(PROCESSENTRY32W) };
	bool found = false;
	if (Process32FirstW(snapshot, &entry)) {
		do {
			std::wstring name = entry.szExeFile;
			std::wstring target = process_name;
			std::transform(name.begin(), name.end(), name.begin(), ::towlower);
			std::transform(target.begin(), target.end(), target.begin(), ::towlower);
			if (name == target) { found = true; break; }
		} while (Process32NextW(snapshot, &entry));
	}
	CloseHandle(snapshot);
	return found;
}

bool kill_process_by_name(const std::wstring& process_name) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) return false;
	PROCESSENTRY32W entry{ sizeof(PROCESSENTRY32W) };
	bool killed = false;
	if (Process32FirstW(snapshot, &entry)) {
		do {
			std::wstring name = entry.szExeFile;
			std::wstring target = process_name;
			std::transform(name.begin(), name.end(), name.begin(), ::towlower);
			std::transform(target.begin(), target.end(), target.begin(), ::towlower);
			if (name == target) {
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
				if (hProcess) {
					if (TerminateProcess(hProcess, 0)) killed = true;
					CloseHandle(hProcess);
				}
			}
		} while (Process32NextW(snapshot, &entry));
	}
	CloseHandle(snapshot);
	return killed;
}

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
	constexpr DWORD kIoctlBusenumPlayMouseMove = 0x2A2010;
	DWORD bytes_returned;
	return DeviceIoControl(device_, kIoctlBusenumPlayMouseMove, const_cast<MouseReport*>(&report), sizeof(MouseReport), nullptr, 0, &bytes_returned, nullptr);
}

bool LogitechDriver::report_keyboard(const KeyboardReport& report) const {
	constexpr DWORD kIoctlBusenumPlayKeyboard = 0x2A200C;
	DWORD bytes_returned;
	return DeviceIoControl(device_, kIoctlBusenumPlayKeyboard, const_cast<KeyboardReport*>(&report), sizeof(KeyboardReport), nullptr, 0, &bytes_returned, nullptr);
}

}
