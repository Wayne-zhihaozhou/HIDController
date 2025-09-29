#include "base.hpp"
#include <windows.h>

namespace Send::Type::Internal {

	bool Base::is_modifier(int vKey) {
		int mods[] = { VK_LCONTROL, VK_RCONTROL, VK_LSHIFT, VK_RSHIFT, VK_LMENU, VK_RMENU, VK_LWIN, VK_RWIN };
		for (int mod : mods)
			if (mod == vKey)
				return true;
		return false;
	}

	void Base::mouse_absolute_to_screen(POINT& absolute) const {
		const static int mainScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		const static int mainScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		absolute.x = MulDiv(absolute.x, mainScreenWidth, 65536);
		absolute.y = MulDiv(absolute.y, mainScreenHeight, 65536);
	}

	void Base::mouse_virtual_desk_absolute_to_screen(POINT& absolute) const {
		const static int virtualDeskWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		const static int virtualDeskHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

		absolute.x = MulDiv(absolute.x, virtualDeskWidth, 65536);
		absolute.y = MulDiv(absolute.y, virtualDeskHeight, 65536);
	}

	void Base::mouse_screen_to_relative(POINT& screen_point) {
		POINT point;
		GetCursorPos(&point);
		screen_point.x -= point.x;
		screen_point.y -= point.y;
	}

	void Base::create_base(decltype(&::GetAsyncKeyState)* get_key_state_fallback) {
		this->get_key_state_fallback = get_key_state_fallback;
	}

	uint32_t Base::send_input(const INPUT inputs[], uint32_t n) {
		uint32_t count = 0;

		for (uint32_t i = 0; i < n; i++) {
			DWORD type = inputs[i].type;

			uint32_t j = i + 1;
			while (j < n && inputs[j].type == type)
				j++;

			switch (type) {
			case INPUT_KEYBOARD:
				count += send_keyboard_input(inputs + i, j - i);
				break;
			case INPUT_MOUSE:
				count += send_mouse_input(inputs + i, j - i);
				break;
			}

			i = j;
		}

		return count;
	}

	uint32_t Base::send_mouse_input(const INPUT inputs[], uint32_t n) {
		uint32_t count = 0;
		for (uint32_t i = 0; i < n; i++)
			count += send_mouse_input(inputs[i].mi);
		return count;
	}

	uint32_t Base::send_keyboard_input(const INPUT inputs[], uint32_t n) {
		uint32_t count = 0;
		for (uint32_t i = 0; i < n; i++)
			count += send_keyboard_input(inputs[i].ki);
		return count;
	}

	SHORT Base::get_key_state(int vKey) {
		return (*get_key_state_fallback)(vKey);
	}

	void Base::sync_key_states() {}

	// -------------------- VirtualKeyStates --------------------

	VirtualKeyStates::VirtualKeyStates(KeyboardModifiers& modifiers, std::mutex& mutex)
		: modifiers(modifiers), mutex(mutex) {}

	void VirtualKeyStates::set_modifier_state(int vKey, bool keydown) {
		switch (vKey) {
#define CODE_GENERATE(vk, member)  \
			case vk:  \
				modifiers.member = keydown;  \
				break;

			CODE_GENERATE(VK_LCONTROL, LCtrl)
				CODE_GENERATE(VK_RCONTROL, RCtrl)
				CODE_GENERATE(VK_LSHIFT, LShift)
				CODE_GENERATE(VK_RSHIFT, RShift)
				CODE_GENERATE(VK_LMENU, LAlt)
				CODE_GENERATE(VK_RMENU, RAlt)
				CODE_GENERATE(VK_LWIN, LGui)
				CODE_GENERATE(VK_RWIN, RGui)
#undef CODE_GENERATE
		}
	}

	SHORT VirtualKeyStates::get_key_state(int vKey) {
		switch (vKey) {
#define CODE_GENERATE(vk, member)  case vk: return modifiers.member << 15;

			CODE_GENERATE(VK_LCONTROL, LCtrl)
				CODE_GENERATE(VK_RCONTROL, RCtrl)
				CODE_GENERATE(VK_LSHIFT, LShift)
				CODE_GENERATE(VK_RSHIFT, RShift)
				CODE_GENERATE(VK_LMENU, LAlt)
				CODE_GENERATE(VK_RMENU, RAlt)
				CODE_GENERATE(VK_LWIN, LGui)
				CODE_GENERATE(VK_RWIN, RGui)
#undef CODE_GENERATE
		default:
			return (*get_key_state_fallback)(vKey);
		}
	}

	void VirtualKeyStates::sync_key_states() {
		std::lock_guard lock(mutex);
#define CODE_GENERATE(vk, member)  modifiers.member = (*get_key_state_fallback)(vk) & 0x8000;

		CODE_GENERATE(VK_LCONTROL, LCtrl)
			CODE_GENERATE(VK_RCONTROL, RCtrl)
			CODE_GENERATE(VK_LSHIFT, LShift)
			CODE_GENERATE(VK_RSHIFT, RShift)
			CODE_GENERATE(VK_LMENU, LAlt)
			CODE_GENERATE(VK_RMENU, RAlt)
			CODE_GENERATE(VK_LWIN, LGui)
			CODE_GENERATE(VK_RWIN, RGui)
#undef CODE_GENERATE
	}

	// -------------------- find_device --------------------

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
