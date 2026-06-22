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

void start_tracking_impl(mouse_move_fn, keyboard_fn, mouse_button_fn);
void stop_tracking_impl();
bool is_tracking_impl();
void get_mouse_delta_impl(long* dx, long* dy);
void get_pressed_keys_impl(uint16_t* keys, uint32_t* count, uint32_t max_count);
