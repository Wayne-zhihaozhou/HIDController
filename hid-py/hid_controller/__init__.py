"""hid_controller - Mouse and keyboard control via Logitech HID reports.

This package provides Python bindings for HIDController, which sends HID
reports directly to the Logitech virtual driver. Before using, ensure
Logitech Gaming Software (LGS) is installed and the virtual driver is active.
"""

from __future__ import annotations

# Enum types for mouse buttons and virtual key codes
from hid_controller.mouse import Mouse
from hid_controller.key import Key

# C extension modules (defer import until first use so enums are available for testing)
_hid_send = None
_raw_input = None


def __getattr__(name):
    global _hid_send, _raw_input
    if name in (
        "move_mouse_relative",
        "move_mouse_absolute",
        "mouse_down",
        "mouse_up",
        "mouse_press",
        "mouse_click",
        "mouse_wheel",
        "set_mouse_move_coefficient",
        "auto_calibrate",
        "disable_mouse_acceleration",
        "enable_mouse_acceleration",
        "key_down",
        "key_up",
        "key_press",
        "key_combo",
        "key_seq",
        "release_all_keys",
    ):
        if _hid_send is None:
            import hid_controller.hid_send as _mod
            _hid_send = _mod
        return getattr(_hid_send, name)
    if name in (
        "start_input_tracking",
        "stop_input_tracking",
        "register_raw_input",
        "is_tracking",
        "get_mouse_delta",
        "get_pressed_keys",
    ):
        if _raw_input is None:
            import hid_controller.raw_input as _mod
            _raw_input = _mod
        return getattr(_raw_input, name)
    # Convenience aliases
    if name == "start_tracking":
        return __getattr__("start_input_tracking")
    if name == "stop_tracking":
        return __getattr__("stop_input_tracking")
    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")


# Aliases for convenience (lazy via __getattr__)
# start_tracking = start_input_tracking (access via getattr)
# stop_tracking = stop_input_tracking (access via getattr)


def __dir__():
    # Include aliases in dir() output
    names = ["Mouse", "Key"] + list(__all__) + ["start_tracking", "stop_tracking"]
    return sorted(set(names))

__all__ = [
    # Mouse / Key enums
    "Mouse",
    "Key",
    # Mouse control
    "move_mouse_relative",
    "move_mouse_absolute",
    "mouse_down",
    "mouse_up",
    "mouse_press",
    "mouse_click",
    "mouse_wheel",
    "set_mouse_move_coefficient",
    "auto_calibrate",
    "disable_mouse_acceleration",
    "enable_mouse_acceleration",
    # Keyboard control
    "key_down",
    "key_up",
    "key_press",
    "key_combo",
    "key_seq",
    "release_all_keys",
    # Input tracking
    "start_input_tracking",
    "stop_input_tracking",
    "register_raw_input",
    "is_tracking",
    "get_mouse_delta",
    "get_pressed_keys",
]

def get_version() -> str:
    """Return the package version string."""
    return "0.1.0"
