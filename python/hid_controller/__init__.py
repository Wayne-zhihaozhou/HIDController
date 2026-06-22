"""hid_controller - Mouse and keyboard control via Logitech HID reports.

This package provides Python bindings for HIDController, which sends HID
reports directly to the Logitech virtual driver. Before using, ensure
Logitech Gaming Software (LGS) is installed and the virtual driver is active.
"""

from __future__ import annotations

# C extension modules
from hid_controller._extension import (
    move_mouse_relative,
    move_mouse_absolute,
    mouse_down,
    mouse_up,
    mouse_press,
    mouse_click,
    mouse_wheel,
    set_mouse_move_coefficient,
    auto_calibrate,
    disable_mouse_acceleration,
    enable_mouse_acceleration,
    key_down,
    key_up,
    key_press,
    key_combo,
    key_seq,
    release_all_keys,
)

from hid_controller.input_tracker import start, stop, is_tracking, get_mouse_delta, get_pressed_keys

__all__ = [
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
    "start_tracking",
    "stop_tracking",
    "is_tracking",
    "get_mouse_delta",
    "get_pressed_keys",
]

# Aliases for convenience
start_tracking = start
stop_tracking = stop


def get_version() -> str:
    """Return the package version string."""
    return "0.1.0"
