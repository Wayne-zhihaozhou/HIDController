"""Mouse button flags for hid_send.mouse_down(), mouse_up(), mouse_click().

Values match the Windows MOUSEEVENTF constants as used in the C++ code.

Examples
--------
>>> from hid_controller import Mouse
>>> from hid_controller.hid_send import mouse_down, mouse_up
>>> mouse_down(Mouse.LEFT)
>>> mouse_up(Mouse.LEFT)
"""

from enum import IntFlag


class Mouse(IntFlag):
    """Mouse button flags.

    Use with hid_send.mouse_down(), mouse_up(), mouse_click().

    Attributes
    ----------
    LEFT : int
        Left button down (0x02).
    LEFTUP : int
        Left button up (0x04).
    RIGHT : int
        Right button down (0x08).
    RIGHTUP : int
        Right button up (0x10).
    MIDDLE : int
        Middle button down (0x20).
    MIDDLEUP : int
        Middle button up (0x40).
    XBUTTON1 : int
        XButton1 down (0x81 = MOUSEEVENTF_XDOWN | XBUTTON1).
    XBUTTON2 : int
        XButton2 down (0x82 = MOUSEEVENTF_XDOWN | XBUTTON2).
    """

    LEFT = 0x02
    LEFTUP = 0x04
    RIGHT = 0x08
    RIGHTUP = 0x10
    MIDDLE = 0x20
    MIDDLEUP = 0x40
    XBUTTON1 = 0x81
    XBUTTON2 = 0x82
