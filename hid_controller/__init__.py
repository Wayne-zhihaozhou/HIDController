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


# 键盘键码常量 - 支持字符串、常量和数字三种方式
class KeyEvent:
    """
    键盘键码常量类。
    
    支持三种方式调用（推荐字符串，常量便于编辑器提示）：
        KeyEvent.ENTER     -> "Enter"      -> 13
        KeyEvent.A         -> "a"          -> 65
        KeyEvent.LCONTROL  -> "lctrl"      -> 162
    
    使用示例:
        hid_controller.key_press("Enter")          # 字符串方式（推荐）
        hid_controller.key_press(KeyEvent.ENTER)   # 常量方式（编辑器提示）
        hid_controller.key_press(13)               # 数字方式
    """
    KEYUP = 0x0002
    EXTENDED_KEY = 0x0001
    
    # ========== 修饰键 ==========
    LCONTROL = 0xA2   # VK_LCONTROL = 162 - 左Ctrl
    RCONTROL = 0xA3   # VK_RCONTROL = 163 - 右Ctrl
    LSHIFT = 0xA0     # VK_LSHIFT = 160 - 左Shift
    RSHIFT = 0xA1     # VK_RSHIFT = 161 - 右Shift
    LMENU = 0xA4      # VK_LMENU = 164 - 左Alt (Left Alt)
    RMENU = 0xA5      # VK_RMENU = 165 - 右Alt (Right Alt)
    LWIN = 0x5B       # VK_LWIN = 91 - 左Win键
    RWIN = 0x5C       # VK_RWIN = 92 - 右Win键
    CONTROL = 0x11    # VK_CONTROL = 17 - Ctrl（通用）
    SHIFT = 0x10      # VK_SHIFT = 16 - Shift（通用）
    MENU = 0x12       # VK_MENU = 18 - Alt（通用）
    
    # ========== 功能键 ==========
    F1 = 0x70         # VK_F1 = 112
    F2 = 0x71
    F3 = 0x72
    F4 = 0x73
    F5 = 0x74
    F6 = 0x75
    F7 = 0x76
    F8 = 0x77
    F9 = 0x78
    F10 = 0x79
    F11 = 0x7A
    F12 = 0x7B
    
    # ========== 控制键 ==========
    RETURN = 0x0D     # VK_RETURN = 13 - Enter
    ESCAPE = 0x1B     # VK_ESCAPE = 27 - Esc
    TAB = 0x09        # VK_TAB = 9 - Tab
    BACK = 0x08       # VK_BACK = 8 - Backspace
    DELETE = 0x2E     # VK_DELETE = 46 - Delete
    INSERT = 0x2D     # VK_INSERT = 45 - Insert
    HOME = 0x24       # VK_HOME = 36 - Home
    END = 0x23        # VK_END = 35 - End
    PRIOR = 0x21      # VK_PRIOR = 33 - Page Up
    NEXT = 0x22       # VK_NEXT = 34 - Page Down
    SPACE = 0x20      # VK_SPACE = 32 - 空格
    
    # ========== 方向键 ==========
    UP = 0x26         # VK_UP = 38 - 上方向键
    DOWN = 0x28       # VK_DOWN = 40 - 下方向键
    LEFT = 0x25       # VK_LEFT = 37 - 左方向键
    RIGHT = 0x27      # VK_RIGHT = 39 - 右方向键
    
    # ========== 状态键 ==========
    CAPITAL = 0x14    # VK_CAPITAL = 20 - Caps Lock
    NUMLOCK = 0x90    # VK_NUMLOCK = 144 - Num Lock
    SCROLL = 0x91     # VK_SCROLL = 145 - Scroll Lock
    
    # ========== 字母 A-Z（与 ASCII 码一致）==========
    A = 0x41; B = 0x42; C = 0x43; D = 0x44; E = 0x45
    F = 0x46; G = 0x47; H = 0x48; I = 0x49; J = 0x4A
    K = 0x4B; L = 0x4C; M = 0x4D; N = 0x4E; O = 0x4F
    P = 0x50; Q = 0x51; R = 0x52; S = 0x53; T = 0x54
    U = 0x55; V = 0x56; W = 0x57; X = 0x58; Y = 0x59; Z = 0x5A
    
    # ========== 数字 0-9（与 ASCII 码一致）==========
    KEY_0 = 0x30; KEY_1 = 0x31; KEY_2 = 0x32; KEY_3 = 0x33; KEY_4 = 0x34
    KEY_5 = 0x35; KEY_6 = 0x36; KEY_7 = 0x37; KEY_8 = 0x38; KEY_9 = 0x39


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
