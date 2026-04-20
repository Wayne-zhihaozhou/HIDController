"""HIDController - Mouse and keyboard control via Logitech HID reports."""


# 鼠标事件常量
class MouseEvent:
    LEFT_DOWN = 0x02
    LEFT_UP = 0x04
    RIGHT_DOWN = 0x08
    RIGHT_UP = 0x10
    MIDDLE_DOWN = 0x20
    MIDDLE_UP = 0x40
    XDOWN = 0x0080
    XUP = 0x0100
    XBUTTON1 = 0x0001
    XBUTTON2 = 0x0002
    MOVE = 0x01
    WHEEL = 0x0800
    ABSOLUTE = 0x8000


# 键盘事件常量
class KeyEvent:
    KEYUP = 0x0002
    EXTENDED_KEY = 0x0001


try:
    from ._extension import (
        auto_calibrate,
        disable_mouse_acceleration,
        enable_mouse_acceleration,
        key_combo,
        key_down,
        key_press,
        key_seq,
        key_up,
        mouse_click,
        mouse_down,
        mouse_up,
        mouse_wheel,
        move_mouse_absolute,
        move_mouse_relative,
        release_all_keys,
        set_mouse_move_coefficient,
    )
except ImportError:

    def _none_func(*args, **kwargs):
        raise RuntimeError(
            "hid_controller extension not compiled. " "Run: pip install . 或 python setup.py build_ext --inplace"
        )

    auto_calibrate = _none_func
    disable_mouse_acceleration = _none_func
    enable_mouse_acceleration = _none_func
    key_combo = _none_func
    key_down = _none_func
    key_press = _none_func
    key_seq = _none_func
    key_up = _none_func
    mouse_click = _none_func
    mouse_down = _none_func
    mouse_up = _none_func
    mouse_wheel = _none_func
    move_mouse_absolute = _none_func
    move_mouse_relative = _none_func
    release_all_keys = _none_func
    set_mouse_move_coefficient = _none_func

__version__ = "1.0.0"
__all__ = [
    "MouseEvent",
    "KeyEvent",
    "auto_calibrate",
    "disable_mouse_acceleration",
    "enable_mouse_acceleration",
    "key_combo",
    "key_down",
    "key_press",
    "key_seq",
    "key_up",
    "mouse_click",
    "mouse_down",
    "mouse_up",
    "mouse_wheel",
    "move_mouse_absolute",
    "move_mouse_relative",
    "release_all_keys",
    "set_mouse_move_coefficient",
]
