"""HIDController - Mouse and keyboard control via Logitech HID reports."""


# 鼠标按钮常量 - 支持字符串和数字两种方式
class MouseEvent:
    """
    鼠标按钮常量类。
    
    支持字符串方式调用（推荐）和数字常量方式：
        MouseEvent.LEFT    -> "left"
        MouseEvent.RIGHT   -> "right" 
        MouseEvent.MIDDLE  -> "middle"
        MouseEvent.XBUTTON1 -> "xbutton1"
        MouseEvent.XBUTTON2 -> "xbutton2"
    
    使用示例:
        hid_controller.mouse_click("left")           # 字符串方式（推荐）
        hid_controller.mouse_click(MouseEvent.LEFT)  # 常量方式
        hid_controller.mouse_click(0x02)             # 数字方式
    """
    # 左键
    LEFT_DOWN = 0x02
    LEFT_UP = 0x04
    LEFT = 0x02  # 左键按下（用于 click/press）
    
    # 右键
    RIGHT_DOWN = 0x08
    RIGHT_UP = 0x10
    RIGHT = 0x08  # 右键按下（用于 click/press）
    
    # 中键
    MIDDLE_DOWN = 0x20
    MIDDLE_UP = 0x40
    MIDDLE = 0x20  # 中键按下（用于 click/press）
    
    # X 按钮（侧边键）
    XDOWN = 0x0080
    XUP = 0x0100
    XBUTTON1 = 0x0001
    XBUTTON2 = 0x0002
    XBUTTON1_DOWN = 0x81   # MOUSEEVENTF_XDOWN | XBUTTON1
    XBUTTON1_UP = 0x81     # MOUSEEVENTF_XUP | XBUTTON1
    XBUTTON2_DOWN = 0x82   # MOUSEEVENTF_XDOWN | XBUTTON2
    XBUTTON2_UP = 0x82     # MOUSEEVENTF_XUP | XBUTTON2
    
    # 移动和滚轮
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
        mouse_press,
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
    mouse_press = _none_func
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
    "mouse_press",
    "mouse_up",
    "mouse_wheel",
    "move_mouse_absolute",
    "move_mouse_relative",
    "release_all_keys",
    "set_mouse_move_coefficient",
]
