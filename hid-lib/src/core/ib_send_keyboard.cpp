// ib_send_keyboard.cpp
#include "pch.h"
#include <logitech.h>
#include <vector>

DLLAPI bool WINAPI key_down(KeyCode vk) {
	auto& logitech = send::Logitech::get_logitech_instance();
	return logitech.send_keyboard_report(vk, true);
}

DLLAPI bool WINAPI key_up(KeyCode vk) {
	auto& logitech = send::Logitech::get_logitech_instance();
	return logitech.send_keyboard_report(vk, false);
}

DLLAPI bool WINAPI key_press(KeyCode vk) {
	auto& logitech = send::Logitech::get_logitech_instance();
	if (!logitech.send_keyboard_report(vk, true)) return false;
	return logitech.send_keyboard_report(vk, false);
}

DLLAPI bool WINAPI key_combo(const std::vector<KeyCode>& keys) {
	auto& logitech = send::Logitech::get_logitech_instance();

	// Press all keys (front to back)
	for (auto vk : keys) {
		if (!logitech.send_keyboard_report(vk, true)) return false;
	}

	// Release all keys in reverse (back to front, so modifiers release last)
	for (auto it = keys.rbegin(); it != keys.rend(); ++it) {
		if (!logitech.send_keyboard_report(*it, false)) return false;
	}

	return true;
}

DLLAPI bool WINAPI key_seq(const std::vector<KeyCode>& keys) {
	auto& logitech = send::Logitech::get_logitech_instance();

	for (auto vk : keys) {
		if (!logitech.send_keyboard_report(vk, true)) return false;
		if (!logitech.send_keyboard_report(vk, false)) return false;
	}

	return true;
}

DLLAPI void WINAPI release_all_keys() {
	auto& logitech = send::Logitech::get_logitech_instance();
	logitech.release_all_keys();
	logitech.release_all_mouse();
}
