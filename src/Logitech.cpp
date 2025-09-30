#include "Logitech.hpp" 
#include <string_view>


namespace Send::Type::Internal {

	Logitech::Logitech() : VirtualKeyStates(keyboard_report.modifiers, keyboard_mutex) {}


	//临时复制?
	Send::Error Logitech::create() {
		return driver.create();
	}

	//临时复制?
	void Logitech::destroy() {
		driver.destroy();
	}

	//临时复制?+修复
	uint32_t Logitech::send_mouse_input(const INPUT inputs[], uint32_t n) {
		uint32_t count = 0;
		for (uint32_t i = 0; i < n; ++i) {
			if (inputs[i].type == INPUT_MOUSE) {
				if (send_mouse_input(inputs[i].mi)) {
					++count;
				}
			}
		}
		return count;
	}

	//临时复制?
	bool Logitech::send_mouse_input(const MOUSEINPUT& mi) {
		return send_mouse_report<MouseReport>(mi);
	}


	//临时复制?
	bool Logitech::send_keyboard_input(const KEYBDINPUT& ki) {
		std::lock_guard lock(keyboard_mutex);

		bool keydown = !(ki.dwFlags & KEYEVENTF_KEYUP);
		if (is_modifier(ki.wVk)) {
			set_modifier_state(ki.wVk, keydown);
		}
		else {
			uint8_t usage = Usb::keyboard_vk_to_usage((uint8_t)ki.wVk);;
			if (keydown) {
				for (int i = 0; i < 6; i++) {
					if (keyboard_report.keys[i] == 0) {
						keyboard_report.keys[i] = usage;
						break;
					}
				}
				//full
			}
			else {
				for (int i = 0; i < 6; i++) {
					if (keyboard_report.keys[i] == usage) {
						keyboard_report.keys[i] = 0;
						//#TODO: move to left?
						break;
					}
				}
			}
		}

		return driver.report_keyboard(keyboard_report);
	}

	//临时复制?
	template <class ReportType>
	bool Logitech::send_mouse_report(const MOUSEINPUT& mi)
	{
		std::lock_guard lock(mouse_mutex);


		ReportType mouse_report{};

		//#TODO: move and then click, or click and then move? former?

		//#TODO: MOUSEEVENTF_MOVE_NOCOALESCE
		if (mi.dwFlags & MOUSEEVENTF_MOVE) {
			POINT move{ mi.dx, mi.dy };
			if (mi.dwFlags & MOUSEEVENTF_ABSOLUTE) {
				if (mi.dwFlags & MOUSEEVENTF_VIRTUALDESK)
					mouse_virtual_desk_absolute_to_screen(move);
				else
					mouse_absolute_to_screen(move);
				mouse_screen_to_relative(move);
			}

			static_assert(std::is_same_v<decltype(mouse_report.x), decltype(mouse_report.y)>);
			using CoordinatesType = decltype(mouse_report.x);
			constexpr auto maxValue = max_value<CoordinatesType>();

			while (abs(move.x) > maxValue || abs(move.y) > maxValue) {
				if (abs(move.x) > maxValue) {
					mouse_report.x = move.x > 0 ? maxValue : -maxValue;
					move.x -= mouse_report.x;
				}
				else {
					mouse_report.x = 0;
				}

				if (abs(move.y) > maxValue) {
					mouse_report.y = move.y > 0 ? maxValue : -maxValue;
					move.y -= mouse_report.y;
				}
				else {
					mouse_report.y = 0;
				}

				driver.report_mouse(mouse_report, compensate_switch = -compensate_switch);
			}

			mouse_report.x = (CoordinatesType)move.x;
			mouse_report.y = (CoordinatesType)move.y;
		}
		else {
			mouse_report.x = 0;
			mouse_report.y = 0;
		}

		if (mi.dwFlags & MOUSEEVENTF_WHEEL) { // TODO MOUSEEVENTF_HWHEEL
			mouse_report.wheel = std::bit_cast<int32_t>(mi.mouseData) > 0 ? 1 : -1;
		}

#define CODE_GENERATE(down, up, member)  \
            if (mi.dwFlags & down || mi.dwFlags & up)  \
                mouse_report.button.##member = mi.dwFlags & down;

		CODE_GENERATE(MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, LButton)  //#TODO: may be switched?
			CODE_GENERATE(MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, RButton)
			CODE_GENERATE(MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, MButton)
#undef CODE_GENERATE
			if (mi.dwFlags & (MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP)) {
				bool down = mi.dwFlags & MOUSEEVENTF_XDOWN;
				switch (mi.mouseData) {
				case XBUTTON1: mouse_report.button.XButton1 = down; break;
				case XBUTTON2: mouse_report.button.XButton2 = down; break;
				}
			}

		return driver.report_mouse(mouse_report, compensate_switch = -compensate_switch);
	}

} // namespace Send::Type::Internal


