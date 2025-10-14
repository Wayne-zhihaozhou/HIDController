//LogitechDriver.hpp
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

namespace Send {
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

		struct MouseReport {
			union {
				MouseButton button;
				uint8_t button_byte;
			};
			int8_t x;
			int8_t y;
			int8_t wheel;
			int8_t unknown_T;

		private:
			void assert_size() { static_assert(sizeof(MouseReport) == 5); }
		};

		struct KeyboardReport {
			union {
				KeyboardModifiers modifiers;
				uint8_t modifiers_byte;
			};
			uint8_t reserved;
			uint8_t keys[6];
		};

	private:
		std::wstring find_device();

	public:
		// 构造函数：初始化句柄
		LogitechDriver() : device(INVALID_HANDLE_VALUE) {}

		// 析构函数：销毁资源
		~LogitechDriver() { destroy(); }

		// 禁止拷贝
		LogitechDriver(const LogitechDriver&) = delete;
		LogitechDriver& operator=(const LogitechDriver&) = delete;

		// 支持移动语义
		LogitechDriver(LogitechDriver&& other) noexcept {
			device = other.device;
			other.device = INVALID_HANDLE_VALUE;
		}
		LogitechDriver& operator=(LogitechDriver&& other) noexcept {
			if (this != &other) {
				destroy();
				device = other.device;
				other.device = INVALID_HANDLE_VALUE;
			}
			return *this;
		}

		bool create();
		void destroy();
		bool report_mouse(const MouseReport& report) const;
		bool report_keyboard(const KeyboardReport& report) const;
		

	};
}




