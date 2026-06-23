// keyboard_map.h
// Maps Windows virtual key codes (KeyCode enum) to USB HID keyboard Usage IDs.
// Single source of truth for VK→HID conversion.
#pragma once
#include <cstdint>
#include "virtual_keys.h"

namespace send::usb {
	// Convert KeyCode to USB HID keyboard Usage ID
	constexpr uint8_t keyboard_vk_to_usage(KeyCode vk) {
		switch (vk) {
		case KeyCode::CANCEL: return 0x9B;
		case KeyCode::BACK: return 0x2A;
		case KeyCode::TAB: return 0x2B;
		case KeyCode::CLEAR: return 0x00;
		case KeyCode::ENTER: return 0x28;
		case KeyCode::PAUSE: return 0x48;
		case KeyCode::CAPS_LOCK: return 0x39;
		case KeyCode::ESCAPE: return 0x29;
		case KeyCode::SPACE: return 0x2C;
		case KeyCode::PRIOR: return 0x4B;
		case KeyCode::NEXT: return 0x4E;
		case KeyCode::END: return 0x4D;
		case KeyCode::HOME: return 0x4A;
		case KeyCode::LEFT: return 0x50;
		case KeyCode::UP: return 0x52;
		case KeyCode::RIGHT: return 0x4F;
		case KeyCode::DOWN: return 0x51;
		case KeyCode::SELECT: return 0x00;
		case KeyCode::PRINT: return 0x00;
		case KeyCode::EXECUTE: return 0x00;
		case KeyCode::SNAPSHOT: return 0x46;
		case KeyCode::INSERT: return 0x49;
		case KeyCode::DEL: return 0x4C;
		case KeyCode::HELP: return 0x00;

		case KeyCode::MULTIPLY: return 0x55;
		case KeyCode::ADD: return 0x57;
		case KeyCode::SEPARATOR: return 0x00;
		case KeyCode::SUBTRACT: return 0x56;
		case KeyCode::DECIMAL: return 0x63;
		case KeyCode::DIVIDE: return 0x54;

		case KeyCode::NUM_LOCK: return 0x53;
		case KeyCode::SCROLL_LOCK: return 0x47;

		case KeyCode::LSHIFT: return 0xE1;
		case KeyCode::RSHIFT: return 0xE5;
		case KeyCode::LCTRL: return 0xE0;
		case KeyCode::RCTRL: return 0xE4;
		case KeyCode::LALT: return 0xE2;
		case KeyCode::RALT: return 0xE6;
		case KeyCode::LWIN: return 0xE3;
		case KeyCode::RWIN: return 0xE7;

		case KeyCode::OEM_1: return 0x33;
		case KeyCode::OEM_PLUS: return 0x2E;
		case KeyCode::OEM_COMMA: return 0x36;
		case KeyCode::OEM_MINUS: return 0x2D;
		case KeyCode::OEM_PERIOD: return 0x37;
		case KeyCode::OEM_2: return 0x38;
		case KeyCode::OEM_3: return 0x35;
		case KeyCode::OEM_4: return 0x2F;
		case KeyCode::OEM_5: return 0x31;
		case KeyCode::OEM_6: return 0x30;
		case KeyCode::OEM_7: return 0x34;
		case KeyCode::OEM_8: return 0x00;
		case KeyCode::OEM_102: return 0x64;

		case KeyCode::APPS: return 0x65;

		default:
			const auto raw = static_cast<uint8_t>(vk);
			if ('A' <= raw && raw <= 'Z')
				return 0x04 + raw - 'A';
			if ('0' <= raw && raw <= '9')
				return raw == '0' ? 0x27 : 0x1E + raw - '1';
			if (KeyCode::NUMPAD0 <= vk && vk <= KeyCode::NUMPAD9)
				return vk == KeyCode::NUMPAD0 ? 0x62 : 0x59 + static_cast<uint8_t>(vk) - static_cast<uint8_t>(KeyCode::NUMPAD1);
			if (KeyCode::F1 <= vk && vk <= KeyCode::F24)
				return vk <= KeyCode::F12
				? 0x3A + static_cast<uint8_t>(vk) - static_cast<uint8_t>(KeyCode::F1)
				: 0x68 + static_cast<uint8_t>(vk) - static_cast<uint8_t>(KeyCode::F13);
			return 0x00;
		}
	}
}
