//Logitech.hpp
#pragma once
#include <windows.h>
#include <string>
#include <mutex>
#include <bit>
#include <cstdint>
#include <cmath>

//#include "base.hpp"
#include "SendTypes.hpp"
#include "KeyboardUtils.hpp"
#include "LogitechDriver.hpp"



namespace Send::Type::Internal {

	//------------------ MaxValue ------------------
	template<typename T> struct MaxValue;
	template<> struct MaxValue<int8_t> { static constexpr int8_t  value = INT8_MAX; };
	template<> struct MaxValue<int16_t> { static constexpr int16_t value = INT16_MAX; };

	template <typename T>
	static constexpr T max_value()
	{
		if constexpr (std::is_same_v<T, int8_t>)
			return INT8_MAX;
		else
		{
			static_assert(std::is_same_v<T, int16_t>, "Unknown type");
			return INT16_MAX;
		}
	}


	class Logitech : public VirtualKeyStates {
	private:
		LogitechDriver driver;
		std::mutex mouse_mutex;
		LogitechDriver::KeyboardReport keyboard_report{};
		std::mutex keyboard_mutex;
		int8_t compensate_switch = -1;

		struct MouseReport {
			union {
				LogitechDriver::MouseButton button;
				uint8_t button_byte;
			};
			int8_t x;
			int8_t y;
			uint8_t wheel;
			uint8_t unknown_T;

		private:
			void assert_size() { static_assert(sizeof(MouseReport) == 5); }
		};

		template <class ReportType>
		bool send_mouse_report(const MOUSEINPUT& mi);

	public:
		Logitech();
		Send::Error create();
		void destroy() override;

		virtual uint32_t send_mouse_input(const INPUT inputs[], uint32_t n) override;
		virtual bool send_mouse_input(const MOUSEINPUT& mi) override;
		bool send_keyboard_input(const KEYBDINPUT& ki) override;

	};

} // namespace Send::Type::Internal

