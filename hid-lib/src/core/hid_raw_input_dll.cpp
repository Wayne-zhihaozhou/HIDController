// hid_raw_input_dll.cpp — DLL wrappers for raw_input functions declared in hid_controller.h
// HIDController project — delegates to the _impl functions defined in input_tracker_impl.cpp
#include <windows.h>
#include "../include/hid_controller.h"
#include "input_tracker_internal.h"

bool WINAPI start_input_tracking(
    mouse_move_callback mc,
    keyboard_callback kc,
    mouse_button_callback mbc,
    mouse_wheel_callback wbc)
{
    start_tracking_impl(
        [mc](uintptr_t h, long dx, long dy) { mc(h, (int32_t)dx, (int32_t)dy); },
        [kc](uintptr_t h, uint16_t vk, bool d) { kc(h, vk, d); },
        [mbc](uintptr_t h, uint32_t b, bool d) { mbc(h, b, d); },
        [wbc](uintptr_t h, int32_t wd, bool horizontal) { wbc(h, wd, horizontal ? 1 : 0); }
    );
    return true;
}

void WINAPI stop_input_tracking()
{
    stop_tracking_impl();
}

bool WINAPI is_tracking()
{
    return is_tracking_impl();
}

std::pair<long, long> WINAPI get_mouse_delta()
{
    auto [ldx, ldy] = get_mouse_delta_impl();
    return {ldx, ldy};
}

std::vector<uint16_t> WINAPI get_pressed_keys()
{
    return get_pressed_keys_impl();
}

bool WINAPI register_raw_input(HWND hwnd)
{
    RAWINPUTDEVICE rid[2];

    rid[0].usUsagePage = 0x01;
    rid[0].usUsage = 0x02;
    rid[0].dwFlags = RIDEV_INPUTSINK;
    rid[0].hwndTarget = hwnd;

    rid[1].usUsagePage = 0x01;
    rid[1].usUsage = 0x06;
    rid[1].dwFlags = RIDEV_INPUTSINK;
    rid[1].hwndTarget = hwnd;

    return RegisterRawInputDevices(rid, 2, sizeof(rid[0])) != FALSE;
}

bool WINAPI get_device_name(uintptr_t device_handle, wchar_t* name, uint32_t* name_length)
{
    std::wstring wname = get_device_name_impl(device_handle);
    if (wname.empty() && *name_length > 0) {
        name[0] = L'\0';
        *name_length = 0;
        return false;
    }
    uint32_t needed = static_cast<uint32_t>(wname.size());
    if (*name_length <= needed) {
        *name_length = needed + 1;
        return false;
    }
    wcsncpy_s(name, *name_length, wname.c_str(), needed);
    *name_length = needed;
    return true;
}
