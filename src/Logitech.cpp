#include "Logitech.hpp"
#include <string_view>

namespace Send::Type::Internal {

    // LogitechDriver 实现
    Error LogitechDriver::create() {
        std::wstring device_name = find_device();
        if (device_name.empty()) return Error::DeviceNotFound;

        device = CreateFileW(
            device_name.c_str(),
            GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL
        );
        return (device == INVALID_HANDLE_VALUE) ? Error::DeviceOpenFailed : Error::Success;
    }

    void LogitechDriver::destroy() {
        CloseHandle(device);
    }


    std::wstring LogitechDriver::find_device() {
        return Internal::find_device([](std::wstring_view sv) {
            using namespace std::literals;

            return (sv.starts_with(L"ROOT#SYSTEM#"sv)
                || sv.starts_with(L"Root#SYSTEM#"sv))
                &&
                (sv.ends_with(L"#{1abc05c0-c378-41b9-9cef-df1aba82b015}"sv)
                    || sv.ends_with(L"#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}"sv)
                    || sv.ends_with(L"#{dfbedcdb-2148-416d-9e4d-cecc2424128c}"sv)
                    || sv.ends_with(L"#{5bada891-842b-4296-a496-68ae931aa16c}"sv));
            });
    }


    bool LogitechDriver::report_keyboard(KeyboardReport report) const {
        DWORD bytes_returned;
        return DeviceIoControl(device, 0x2A200C, &report, sizeof(KeyboardReport),
            nullptr, 0, &bytes_returned, nullptr);
    }

    // Logitech 实现
    Logitech::Logitech() : VirtualKeyStates(keyboard_report.modifiers, keyboard_mutex) {}

    Error Logitech::create() {
        return driver.create();
    }

    void Logitech::destroy() {
        driver.destroy();
    }

    uint32_t Logitech::send_mouse_input(const INPUT inputs[], uint32_t n) {
        return Base::send_mouse_input(inputs, n);
    }

    bool Logitech::send_mouse_input(const MOUSEINPUT& mi) {
        return send_mouse_report<MouseReport>(mi);
    }



    bool Logitech::send_keyboard_input(const KEYBDINPUT& ki) {
        std::lock_guard lock(keyboard_mutex);

        bool keydown = !(ki.dwFlags & KEYEVENTF_KEYUP);
        if (is_modifier(ki.wVk)) {
            set_modifier_state(ki.wVk, keydown);
        }
        else {
            uint8_t usage = Usb::keyboard_vk_to_usage((uint8_t)ki.wVk);
            if (keydown) {
                for (int i = 0; i < 6; i++) {
                    if (keyboard_report.keys[i] == 0) {
                        keyboard_report.keys[i] = usage;
                        break;
                    }
                }
                // full
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




} // namespace Send::Type::Internal
