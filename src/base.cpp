//base.cpp
#include"pch.h"
#include"base.hpp"

namespace Send::Internal {

	//// 判断某个虚拟键码是否为修饰键（Ctrl/Shift/Alt/Win）
	//bool Base::is_modifier(int vKey) {
	//	int mods[] = { VK_LCONTROL, VK_RCONTROL, VK_LSHIFT, VK_RSHIFT, VK_LMENU, VK_RMENU, VK_LWIN, VK_RWIN };
	//	for (int mod : mods)
	//		if (mod == vKey)
	//			return true;
	//	return false;
	//}

	// 将鼠标绝对坐标转换为主屏幕坐标
	void Base::mouse_absolute_to_screen(POINT& absolute) const {
		const static int mainScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		const static int mainScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		absolute.x = MulDiv(absolute.x, mainScreenWidth, 65536);
		absolute.y = MulDiv(absolute.y, mainScreenHeight, 65536);
	}

	// 将鼠标绝对坐标转换为虚拟桌面屏幕坐标
	void Base::mouse_virtual_desk_absolute_to_screen(POINT& absolute) const {
		const static int virtualDeskWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		const static int virtualDeskHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

		absolute.x = MulDiv(absolute.x, virtualDeskWidth, 65536);
		absolute.y = MulDiv(absolute.y, virtualDeskHeight, 65536);
	}

	// 将屏幕坐标转换为相对当前鼠标位置的偏移
	void Base::mouse_screen_to_relative(POINT& screen_point) {
		POINT point;
		GetCursorPos(&point);
		screen_point.x -= point.x;
		screen_point.y -= point.y;
	}

	// 初始化Base类，设置备用键盘状态函数
	void Base::create_base(decltype(&::GetAsyncKeyState)* get_key_state_fallback) {
		this->get_key_state_fallback = get_key_state_fallback;
	}

	// 发送一组输入事件（键盘或鼠标）
	//uint32_t Base::send_input(const INPUT inputs[], uint32_t n) {
	//	uint32_t count = 0;

	//	for (uint32_t i = 0; i < n; i++) {
	//		DWORD type = inputs[i].type;

	//		uint32_t j = i + 1;
	//		while (j < n && inputs[j].type == type)
	//			j++;

	//		switch (type) {
	//		case INPUT_KEYBOARD:
	//			count += send_keyboard_input(inputs + i, j - i);
	//			break;
	//		case INPUT_MOUSE:
	//			count += send_mouse_input(inputs + i, j - i);
	//			break;
	//		}

	//		i = j;
	//	}

	//	return count;
	//}

	// 批量发送鼠标输入事件
	//uint32_t Base::send_mouse_input(const INPUT inputs[], uint32_t n) {
	//	uint32_t count = 0;
	//	for (uint32_t i = 0; i < n; i++)
	//		count += send_mouse_input(inputs[i].mi);
	//	return count;
	//}

	// 批量发送键盘输入事件
	uint32_t Base::send_keyboard_input(const INPUT inputs[], uint32_t n) {
		uint32_t count = 0;
		for (uint32_t i = 0; i < n; i++)
			count += send_keyboard_report(inputs[i].ki);
		return count;
	}

	// 获取某个按键的状态（按下/释放）
	SHORT Base::get_key_state(int vKey) {
		return (*get_key_state_fallback)(vKey);
	}

	// -------------------- find_device --------------------

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

}


namespace Send {
	std::unique_ptr<Send::Internal::Base> g_send = nullptr;
}