// LogitechDriver.cpp
#include"pch.h"
#include"LogitechDriver.hpp"
#include "Logitech.hpp"
#include <tlhelp32.h> 


namespace Send {
	/** 从系统 PATH 中查找程序可执行文件的完整路径 */
	std::wstring FindExecutableInPath(const std::wstring& exe_name = L"LCore.exe") {
		wchar_t* path_env = nullptr;
		size_t len = 0;

		_wdupenv_s(&path_env, &len, L"PATH");
		if (!path_env) {
			return L"";
		}

		std::wstring path_str(path_env);
		free(path_env);

		std::wstringstream ss(path_str);
		std::wstring folder;
		while (std::getline(ss, folder, L';')) {
			std::filesystem::path full_path = folder;
			full_path /= exe_name;
			if (std::filesystem::exists(full_path)) {
				return full_path.wstring();
			}
		}
		return L"";
	}

	/** 启动指定路径的进程 */
	bool StartProcess(const std::wstring& process_path) {
		STARTUPINFOW si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// 调用 CreateProcessW 启动进程
		if (!CreateProcessW(
			process_path.c_str(),  // 可执行文件路径
			nullptr,               // 命令行参数
			nullptr,               // 默认安全属性
			nullptr,               // 默认线程安全属性
			FALSE,                 // 不继承句柄
			0,                     // 默认创建标志
			nullptr,               // 使用父进程的环境
			nullptr,               // 使用父进程的工作目录
			&si,
			&pi)) {
			return false;
		}

		// 启动成功，关闭句柄
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}

	/** 检查进程是否存在 */
	bool IsProcessRunning(const std::wstring& process_name) {
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE) {
			return false;
		}

		PROCESSENTRY32W entry;
		entry.dwSize = sizeof(PROCESSENTRY32W);
		bool found = false;

		if (Process32FirstW(snapshot, &entry)) {
			do {
				std::wstring exe_name = entry.szExeFile;
				std::wstring target = process_name;

				std::transform(exe_name.begin(), exe_name.end(), exe_name.begin(), ::towlower);
				std::transform(target.begin(), target.end(), target.begin(), ::towlower);

				if (exe_name == target) {
					found = true;
					break;
				}
			} while (Process32NextW(snapshot, &entry));
		}

		CloseHandle(snapshot);
		return found;
	}

	/** 查找并终止指定进程 */
	bool KillProcessByName(const std::wstring& process_name) {
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE) {
			return false;
		}

		PROCESSENTRY32W entry;
		entry.dwSize = sizeof(PROCESSENTRY32W);
		bool killed = false;

		if (Process32FirstW(snapshot, &entry)) {
			do {
				std::wstring exe_name = entry.szExeFile;
				std::wstring target = process_name;

				std::transform(exe_name.begin(), exe_name.end(), exe_name.begin(), ::towlower);
				std::transform(target.begin(), target.end(), target.begin(), ::towlower);

				if (exe_name == target) {
					HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
					if (hProcess) {
						if (TerminateProcess(hProcess, 0)) {
							killed = true;
						}
						CloseHandle(hProcess);
					}
				}
			} while (Process32NextW(snapshot, &entry));
		}

		CloseHandle(snapshot);
		return killed;
	}

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
		return Send::find_device([](std::wstring_view sv) {
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

		//const std::wstring& exe_name = FindExecutableInPath();
		//if (exe_name.empty()) {
		//	printf("Cannot find LCore.exe in PATH\n");
		//	return false;
		//}

		//// 启动 Logitech 驱动程序
		//if (!StartProcess(exe_name)) {
		//	return false;
		//}

		//// 等待驱动程序启动
		//while (!IsProcessRunning(exe_name)) {
		//	Sleep(100);
		//}
		//KillProcessByName(exe_name);

		STARTUPINFOW si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
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
