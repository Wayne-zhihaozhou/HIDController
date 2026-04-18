"""
HIDController Python 库函数文档

这个库提供了对HID设备（鼠标和键盘）的控制功能。

函数列表：
- MouseMoveRelative(dx, dy): 相对鼠标移动
- MouseMoveAbsolute(x, y): 绝对鼠标移动
- MouseDown(button): 鼠标按下
- MouseUp(button): 鼠标抬起
- MouseClick(button): 鼠标点击
- MouseWheel(movement): 鼠标滚轮
- KeyDown(vk): 键盘按下
- KeyUp(vk): 键盘抬起
- KeyPress(vk): 键盘按下和抬起
- KeyCombo(keys): 组合键
- KeySeq(keys): 键盘序列
- release_all_keys(): 释放所有按键

使用示例：
    import hid_controller
    
    # 鼠标操作
    hid_controller.MouseMoveRelative(100, 50)  # 相对移动鼠标
    hid_controller.MouseClick(hid_controller.MOUSE_LEFT)  # 左键点击
    hid_controller.MouseWheel(120)  # 滚动鼠标滚轮
    
    # 键盘操作
    hid_controller.KeyPress('A')  # 按下A键 (使用字符)
    hid_controller.KeyPress(hid_controller.VK_RETURN)  # 按下回车键 (使用虚拟键码宏)
    hid_controller.KeyCombo([hid_controller.VK_SHIFT, 'A'])  # 按下Shift+A组合键

按钮和按键代码说明：
鼠标按钮代码：
    MOUSE_LEFT - 左键 (0x01)
    MOUSE_RIGHT - 右键 (0x02)
    MOUSE_MIDDLE - 中键 (0x04)
    MOUSE_X1 - X键1 (0x08)
    MOUSE_X2 - X键2 (0x10)

键盘按键代码（虚拟键码/字符）：
    'A'-'Z' - 字母键 (使用字符表示)
    '0'-'9' - 数字键 (使用字符表示)
    VK_SHIFT - Shift键 (0x10)
    VK_CONTROL - Ctrl键 (0x11)
    VK_MENU - Alt键 (0x12)
    VK_RETURN - 回车键 (0x0D)
    VK_ESCAPE - ESC键 (0x1B)
    VK_SPACE - 空格键 (0x20)
    VK_BACK - 退格键 (0x08)
    VK_TAB - Tab键 (0x09)
    VK_PRIOR - Page Up (0x21)
    VK_NEXT - Page Down (0x22)
    VK_END - End (0x23)
    VK_HOME - Home (0x24)
    VK_LEFT - 左箭头 (0x25)
    VK_UP - 上箭头 (0x26)
    VK_RIGHT - 右箭头 (0x27)
    VK_DOWN - 下箭头 (0x28)
    VK_F1-VK_F24 - F1-F24键 (0x70-0x83)
    VK_NUMPAD0-VK_NUMPAD9 - 小键盘数字键 (0x60-0x69)
    VK_MULTIPLY - 小键盘乘号 (0x6A)
    VK_ADD - 小键盘加号 (0x6B)
    VK_SUBTRACT - 小键盘减号 (0x6D)
    VK_DECIMAL - 小键盘小数点 (0x6E)
    VK_DIVIDE - 小键盘除号 (0x6F)
    VK_NUMLOCK - 数字锁定键 (0x90)
    VK_SCROLL - 滚动锁定键 (0x91)
    VK_OEM_1 - 分号键 ';:' (0xBA)
    VK_OEM_PLUS - 加号键 '+' (0xBB)
    VK_OEM_COMMA - 逗号键 ',' (0xBC)
    VK_OEM_MINUS - 减号键 '-' (0xBD)
    VK_OEM_PERIOD - 句号键 '.' (0xBE)
    VK_OEM_2 - 反斜杠键 '/?' (0xBF)
    VK_OEM_3 - 波浪号键 '`~' (0xC0)
    VK_OEM_4 - 左方括号 '[' (0xDB)
    VK_OEM_5 - 反斜杠键 '\|' (0xDC)
    VK_OEM_6 - 右方括号 ']' (0xDD)
    VK_OEM_7 - 单引号键 ''"' (0xDE)
    VK_OEM_102 - 右斜杠键 "<>" (0xE2)
"""

# 鼠标控制常量
MOUSE_LEFT = 1
MOUSE_RIGHT = 2
MOUSE_MIDDLE = 4
MOUSE_X1 = 8
MOUSE_X2 = 16

# 键盘控制常量
VK_SHIFT = 0x10
VK_CONTROL = 0x11
VK_MENU = 0x12
VK_RETURN = 0x0D
VK_ESCAPE = 0x1B
VK_SPACE = 0x20
VK_BACK = 0x08
VK_TAB = 0x09
VK_PRIOR = 0x21
VK_NEXT = 0x22
VK_END = 0x23
VK_HOME = 0x24
VK_LEFT = 0x25
VK_UP = 0x26
VK_RIGHT = 0x27
VK_DOWN = 0x28
VK_F1 = 0x70
VK_F2 = 0x71
VK_F3 = 0x72
VK_F4 = 0x73
VK_F5 = 0x74
VK_F6 = 0x75
VK_F7 = 0x76
VK_F8 = 0x77
VK_F9 = 0x78
VK_F10 = 0x79
VK_F11 = 0x7A
VK_F12 = 0x7B
VK_F13 = 0x7C
VK_F14 = 0x7D
VK_F15 = 0x7E
VK_F16 = 0x7F
VK_F17 = 0x80
VK_F18 = 0x81
VK_F19 = 0x82
VK_F20 = 0x83
VK_F21 = 0x84
VK_F22 = 0x85
VK_F23 = 0x86
VK_F24 = 0x87
VK_NUMPAD0 = 0x60
VK_NUMPAD1 = 0x61
VK_NUMPAD2 = 0x62
VK_NUMPAD3 = 0x63
VK_NUMPAD4 = 0x64
VK_NUMPAD5 = 0x65
VK_NUMPAD6 = 0x66
VK_NUMPAD7 = 0x67
VK_NUMPAD8 = 0x68
VK_NUMPAD9 = 0x69
VK_MULTIPLY = 0x6A
VK_ADD = 0x6B
VK_SEPARATOR = 0x6C
VK_SUBTRACT = 0x6D
VK_DECIMAL = 0x6E
VK_DIVIDE = 0x6F
VK_NUMLOCK = 0x90
VK_SCROLL = 0x91
VK_OEM_1 = 0xBA
VK_OEM_PLUS = 0xBB
VK_OEM_COMMA = 0xBC
VK_OEM_MINUS = 0xBD
VK_OEM_PERIOD = 0xBE
VK_OEM_2 = 0xBF
VK_OEM_3 = 0xC0
VK_OEM_4 = 0xDB
VK_OEM_5 = 0xDC
VK_OEM_6 = 0xDD
VK_OEM_7 = 0xDE
VK_OEM_102 = 0xE2

# 鼠标控制函数
def MouseMoveRelative(dx, dy):
    """
    相对鼠标移动
    
    Args:
        dx (int): X轴相对移动距离
        dy (int): Y轴相对移动距离
    
    Returns:
        bool: 操作是否成功
    """
    pass

def MouseMoveAbsolute(x, y):
    """
    绝对鼠标移动
    
    Args:
        x (int): X轴绝对位置 (0-65535)
        y (int): Y轴绝对位置 (0-65535)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def MouseDown(button):
    """
    鼠标按下
    
    Args:
        button (int): 按钮代码 (MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE, MOUSE_X1, MOUSE_X2)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def MouseUp(button):
    """
    鼠标抬起
    
    Args:
        button (int): 按钮代码 (MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE, MOUSE_X1, MOUSE_X2)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def MouseClick(button):
    """
    鼠标点击（按下然后抬起）
    
    Args:
        button (int): 按钮代码 (MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE, MOUSE_X1, MOUSE_X2)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def MouseWheel(movement):
    """
    鼠标滚轮
    
    Args:
        movement (int): 滚动距离 (通常为120的倍数)
    
    Returns:
        bool: 操作是否成功
    """
    pass

# 键盘控制函数
def KeyDown(vk):
    """
    键盘按下
    
    Args:
        vk (int/str): 虚拟键码或字符 (VK code 或 字符)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeyUp(vk):
    """
    键盘抬起
    
    Args:
        vk (int/str): 虚拟键码或字符 (VK code 或 字符)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeyPress(vk):
    """
    键盘按下和抬起
    
    Args:
        vk (int/str): 虚拟键码或字符 (VK code 或 字符)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeyCombo(keys):
    """
    组合键
    
    Args:
        keys (list): 虚拟键码或字符列表 (VK codes 或 字符)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeySeq(keys):
    """
    键盘序列
    
    Args:
        keys (list): 虚拟键码或字符列表 (VK codes 或 字符)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def release_all_keys():
    """
    释放所有按键
    
    Returns:
        bool: 操作是否成功
    """
    pass
