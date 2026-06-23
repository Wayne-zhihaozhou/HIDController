// mouse_button.h — Mouse button enum for HIDController
// Users choose from named buttons instead of remembering MOUSEEVENTF macros.
#pragma once
#include <cstdint>

enum class MouseButton : uint16_t {
    LEFT     = 1,
    RIGHT    = 2,
    MIDDLE   = 3,
    X1       = 4,
    X2       = 5,
};
