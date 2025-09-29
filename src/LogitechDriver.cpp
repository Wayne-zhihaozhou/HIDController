
#include <string_view>
#include <SendTypes.hpp>
#include "LogitechDriver.hpp"


namespace Send::Type::Internal {

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
        if (device != INVALID_HANDLE_VALUE) {
            CloseHandle(device);
            device = INVALID_HANDLE_VALUE;
        }
    }

    std::wstring LogitechDriver::find_device() {
        return Internal::find_device([](std::wstring_view sv) {
            using namespace std::literals;
            return (sv.starts_with(L"ROOT#SYSTEM#"sv) || sv.starts_with(L"Root#SYSTEM#"sv)) &&
                (sv.ends_with(L"#{1abc05c0-c378-41b9-9cef-df1aba82b015}"sv) ||
                    sv.ends_with(L"#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}"sv) ||
                    sv.ends_with(L"#{dfbedcdb-2148-416d-9e4d-cecc2424128c}"sv) ||
                    sv.ends_with(L"#{5bada891-842b-4296-a496-68ae931aa16c}"sv));
            });
    }

    bool LogitechDriver::report_keyboard(KeyboardReport report) const {
        DWORD bytes_returned;
        return DeviceIoControl(device, 0x2A200C, &report, sizeof(KeyboardReport),
            nullptr, 0, &bytes_returned, nullptr);
    }

}  // namespace Send::Type::Internal
