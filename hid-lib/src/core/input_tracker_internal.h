// input_tracker_internal.h — Pure C++ input tracking (no pybind11)
// HIDController project — Windows RAW INPUT API keyboard and mouse event detection
#pragma once
#include <windows.h>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <set>
#include <functional>

using mouse_move_fn = std::function<void(uintptr_t, long, long)>;
using keyboard_fn = std::function<void(uintptr_t, uint16_t, bool)>;
using mouse_button_fn = std::function<void(uintptr_t, uint32_t, bool)>;
using mouse_wheel_fn = std::function<void(uintptr_t, int32_t, bool horizontal)>;

void start_tracking_impl(mouse_move_fn, keyboard_fn, mouse_button_fn, mouse_wheel_fn);
void stop_tracking_impl();
bool is_tracking_impl();
std::pair<long, long> get_mouse_delta_impl();
std::vector<uint16_t> get_pressed_keys_impl();
std::wstring get_device_name_impl(uintptr_t device_handle);
