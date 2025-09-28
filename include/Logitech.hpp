#pragma once
#include <windows.h>
#include <string>
#include <mutex>
#include <bit>
#include <cstdint>
#include <cmath>

#include "base.hpp"
#include "usb.hpp"
#include "InputSimulator.hpp"
#include "basic_type.hpp"

namespace Send::Type::Internal {

    //------------------ MaxValue ------------------
    template<typename T> struct MaxValue;
    template<> struct MaxValue<int8_t> { static constexpr int8_t  value = INT8_MAX; };
    template<> struct MaxValue<int16_t> { static constexpr int16_t value = INT16_MAX; };

    //------------------ LogitechDriver ------------------
    class LogitechDriver {
        HANDLE device;
        bool has_acceleration;

    public:
        Error create();
        void destroy();

    private:
        int get_has_acceleration();
        std::wstring find_device();

    public:
        struct MouseButton {
            bool LButton : 1;
            bool RButton : 1;
            bool MButton : 1;
            bool XButton1 : 1;
            bool XButton2 : 1;
            bool unknown : 3;
        };

        template <class ReportType>
        bool report_mouse(ReportType report, int8_t compensate_switch) const {
            constexpr DWORD IOCTL_BUSENUM_PLAY_MOUSEMOVE = 0x2A2010;
            DWORD bytes_returned;

            //if (has_acceleration && (report.x || report.y)) {
            //    ReportType tmp = report;
            //    tmp.x = tmp.y = compensate_switch;
            //    DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &tmp, sizeof(ReportType), nullptr, 0, &bytes_returned, nullptr);

            //    report.x = compensate_lgs_acceleration(report.x);
            //    report.y = compensate_lgs_acceleration(report.y);
            //}

            return DeviceIoControl(device, IOCTL_BUSENUM_PLAY_MOUSEMOVE, &report, sizeof(ReportType), nullptr, 0, &bytes_returned, nullptr);
        }

        struct KeyboardReport {
            union {
                KeyboardModifiers modifiers;
                uint8_t modifiers_byte;
            };
            uint8_t reserved;
            uint8_t keys[6];
        };

        bool report_keyboard(KeyboardReport report) const;
    };

    //------------------ Logitech ------------------
    class Logitech : public VirtualKeyStates {
    public:
        Logitech();
        Error create();
        void destroy() override;

        uint32_t send_mouse_input(const INPUT inputs[], uint32_t n) override;
        bool send_mouse_input(const MOUSEINPUT& mi) override;
        bool send_keyboard_input(const KEYBDINPUT& ki) override;

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
        bool send_mouse_report(const MOUSEINPUT& mi) {
            std::lock_guard lock(mouse_mutex);

            ReportType mouse_report{};
            int8_t local_compensate = -compensate_switch;

            POINT move{ mi.dx, mi.dy };
            if (mi.dwFlags & MOUSEEVENTF_MOVE) {
                if (mi.dwFlags & MOUSEEVENTF_ABSOLUTE) {
                    if (mi.dwFlags & MOUSEEVENTF_VIRTUALDESK)
                        mouse_virtual_desk_absolute_to_screen(move);
                    else
                        mouse_absolute_to_screen(move);
                    mouse_screen_to_relative(move);
                }

                using CoordinatesType = decltype(mouse_report.x);
                constexpr auto maxValue = MaxValue<CoordinatesType>::value;

                while (std::abs(move.x) > maxValue || std::abs(move.y) > maxValue) {
                    if (std::abs(move.x) > maxValue) {
                        mouse_report.x = move.x > 0 ? maxValue : -maxValue;
                        move.x -= mouse_report.x;
                    }
                    else {
                        mouse_report.x = 0;
                    }
                    if (std::abs(move.y) > maxValue) {
                        mouse_report.y = move.y > 0 ? maxValue : -maxValue;
                        move.y -= mouse_report.y;
                    }
                    else {
                        mouse_report.y = 0;
                    }

                    driver.report_mouse(mouse_report, local_compensate);
                }

                mouse_report.x = static_cast<CoordinatesType>(move.x);
                mouse_report.y = static_cast<CoordinatesType>(move.y);
            }

            if (mi.dwFlags & MOUSEEVENTF_WHEEL) {
                mouse_report.wheel = std::bit_cast<int32_t>(mi.mouseData) > 0 ? 1 : -1;
            }

#define CODE_GENERATE(down, up, member)  \
        if (mi.dwFlags & down || mi.dwFlags & up)  \
            mouse_report.button.member = mi.dwFlags & down;

            CODE_GENERATE(MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, LButton)
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

            return driver.report_mouse(mouse_report, local_compensate);
        }
    };

} // namespace Send::Type::Internal
