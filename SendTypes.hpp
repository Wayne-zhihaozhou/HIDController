// SendTypes.hpp
#pragma once
#include <cstdint>

namespace Send {

    enum class Error : uint32_t {
        Success,
        InvalidArgument,
        LibraryNotFound,
        LibraryLoadFailed,
        LibraryError,
        DeviceCreateFailed,
        DeviceNotFound,
        DeviceOpenFailed
    };

    enum class SendType : uint32_t {
        AnyDriver = 0,
        SendInput = 1,
        Logitech = 2,
        LogitechGHubNew = 6,
        Razer = 3,
        DD = 4,
        MouClassInputInjection = 5
    };

    using InitFlags = const uint32_t;

    struct Init {
        using T = InitFlags;
    };

    enum class HookCode : uint32_t {
        Off,
        On,
        InitOnly,
        Destroy
    };

} 
