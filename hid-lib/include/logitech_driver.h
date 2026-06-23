// logitech_driver.h
#pragma once
#pragma comment(lib, "ntdll.lib")


extern "C" {
	constexpr NTSTATUS STATUS_SUCCESS = 0x00000000;
	constexpr NTSTATUS STATUS_MORE_ENTRIES = 0x00000105;
	constexpr ACCESS_MASK kDirectoryQuery = 0x0001;

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

namespace send {
	class LogitechDriver {
	public:
		HANDLE device_;

		enum class MouseButtonFlag : uint8_t {
			Left   = 0x01,
			Right  = 0x02,
			Middle = 0x04,
			X1     = 0x08,
			X2     = 0x10,
		};

		enum class KeyboardModifierFlag : uint8_t {
			LCtrl  = 0x01,
			LShift = 0x02,
			LAlt   = 0x04,
			LGui   = 0x08,
			RCtrl  = 0x10,
			RShift = 0x20,
			RAlt   = 0x40,
			RGui   = 0x80,
		};

		struct MouseReport {
			uint8_t button_byte_;
			int8_t x_;
			int8_t y_;
			int8_t wheel_;
			int8_t unknown_t_;
		};

		struct KeyboardReport {
			uint8_t modifiers_byte_;
			uint8_t reserved_;
			uint8_t keys_[6];
		};

	private:
		std::wstring find_device();

	public:
		// 构造函数：初始化驱动
		LogitechDriver() : device_(INVALID_HANDLE_VALUE) {}

		// 析构函数：释放资源
		~LogitechDriver() { destroy(); }

		// 禁止拷贝
		LogitechDriver(const LogitechDriver&) = delete;
		LogitechDriver& operator=(const LogitechDriver&) = delete;

		// 支持移动语义
		LogitechDriver(LogitechDriver&& other) noexcept {
			device_ = other.device_;
			other.device_ = INVALID_HANDLE_VALUE;
		}
		LogitechDriver& operator=(LogitechDriver&& other) noexcept {
			if (this != &other) {
				destroy();
				device_ = other.device_;
				other.device_ = INVALID_HANDLE_VALUE;
			}
			return *this;
		}

		bool create();
		void destroy();
		bool report_mouse(const MouseReport& report) const;
		bool report_keyboard(const KeyboardReport& report) const;


	};
	static_assert(sizeof(LogitechDriver::MouseReport) == 5, "MouseReport must be 5 bytes");
}
