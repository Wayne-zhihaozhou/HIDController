#pragma once
#include <cstdint>

namespace Send::Type::Internal::Usb {

	/**
	 * @brief ½«ĞéÄâ¼üÂëÓ³ÉäÎª USB Usage
	 * @param vk ĞéÄâ¼üÂë
	 * @return uint8_t USB usage
	 */
	uint8_t keyboard_vk_to_usage(uint8_t vk);

	/**
	 * @brief ÅĞ¶ÏÊÇ·ñÊÇ modifier ¼ü£¨Ctrl/Shift/Alt/Win£©
	 * @param vk ĞéÄâ¼üÂë
	 * @return true ÊÇ modifier
	 */
	bool is_modifier(uint8_t vk);

}  // namespace Send::Type::Internal::Usb
