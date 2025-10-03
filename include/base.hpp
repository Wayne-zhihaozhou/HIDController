//base.hpp
#pragma once
#pragma comment(lib, "ntdll.lib")


extern "C" {
	constexpr NTSTATUS STATUS_SUCCESS = 0x00000000;
	constexpr NTSTATUS STATUS_MORE_ENTRIES = 0x00000105;
	constexpr NTSTATUS STATUS_BUFFER_TOO_SMALL = 0xC0000023;
	constexpr ACCESS_MASK DIRECTORY_QUERY = 0x0001;

	NTSTATUS WINAPI NtOpenDirectoryObject(
		_Out_ PHANDLE            DirectoryHandle,
		_In_  ACCESS_MASK        DesiredAccess,
		_In_  POBJECT_ATTRIBUTES ObjectAttributes
	);

	typedef struct _OBJECT_DIRECTORY_INFORMATION {
		UNICODE_STRING Name;
		UNICODE_STRING TypeName;
	} OBJECT_DIRECTORY_INFORMATION, * POBJECT_DIRECTORY_INFORMATION;

	NTSTATUS WINAPI NtQueryDirectoryObject(
		_In_      HANDLE  DirectoryHandle,
		_Out_opt_ PVOID   Buffer,
		_In_      ULONG   Length,
		_In_      BOOLEAN ReturnSingleEntry,
		_In_      BOOLEAN RestartScan,
		_Inout_   PULONG  Context,
		_Out_opt_ PULONG  ReturnLength
	);
}



namespace Send::Internal {

	class Base {
	protected:
		decltype(&::GetAsyncKeyState)* get_key_state_fallback;

		//static bool is_modifier(int vKey);
		void mouse_absolute_to_screen(POINT& absolute) const;
		void mouse_virtual_desk_absolute_to_screen(POINT& absolute) const;
		static void mouse_screen_to_relative(POINT& screen_point);

	public:
		Base();
		virtual ~Base() = default;
		virtual void destroy() = 0;

		virtual bool send_keyboard_report(const KEYBDINPUT& ki) = 0;


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

	class VirtualKeyStates : public Base {
		KeyboardModifiers& modifiers;
		std::mutex& mutex;

	protected:
		VirtualKeyStates(KeyboardModifiers& modifiers, std::mutex& mutex);
		void set_modifier_state(int vKey, bool keydown);

	public:
		SHORT get_key_state(int vKey) override;
		//void sync_key_states() override;
	};

	std::wstring find_device(std::function<bool(std::wstring_view name)> p);
}


namespace Send {
	extern std::unique_ptr<Send::Internal::Base> g_send;
}