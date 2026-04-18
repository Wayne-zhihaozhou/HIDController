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
    'shift' - Shift键
    'ctrl' - Ctrl键
    'alt' - Alt键
    'space' - 空格键
    'enter' - 回车键
    'escape' - ESC键
    'tab' - Tab键
"""



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
        vk (int/str): 虚拟键码或字符或特殊字符串表述 (VK code 或 字符 或 特殊字符串表述)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeyUp(vk):
    """
    键盘抬起
    
    Args:
        vk (int/str): 虚拟键码或字符或特殊字符串表述 (VK code 或 字符 或 特殊字符串表述)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeyPress(vk):
    """
    键盘按下和抬起
    
    Args:
        vk (int/str): 虚拟键码或字符或特殊字符串表述 (VK code 或 字符 或 特殊字符串表述)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeyCombo(keys):
    """
    组合键
    
    Args:
        keys (list): 虚拟键码或字符或特殊字符串表述列表 (VK codes 或 字符 或 特殊字符串表述)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeySeq(keys):
    """
    键盘序列
    
    Args:
        keys (list): 虚拟键码或字符或特殊字符串表述列表 (VK codes 或 字符 或 特殊字符串表述)
    
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
