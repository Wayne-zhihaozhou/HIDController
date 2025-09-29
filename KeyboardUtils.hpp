#pragma once
#include <cstdint>

namespace Send::Type::Internal::Usb {


	uint8_t keyboard_vk_to_usage(uint8_t vk);

	bool is_modifier(uint8_t vk);

}  // namespace Send::Type::Internal::Usb
