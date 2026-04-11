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
    hid_controller.MouseClick(1)  # 左键点击
    hid_controller.MouseWheel(120)  # 滚动鼠标滚轮
    
    # 键盘操作
    hid_controller.KeyPress(0x41)  # 按下A键 (VK_A)
    hid_controller.KeyCombo([0x10, 0x41])  # 按下Shift+A组合键

按钮和按键代码说明：
鼠标按钮代码：
    1 - 左键
    2 - 右键
    4 - 中键
    8 - X键

键盘按键代码（虚拟键码）：
    0x41 - A键
    0x42 - B键
    ...
    0x10 - Shift键
    0x11 - Ctrl键
    0x12 - Alt键
    0x0D - 回车键
    0x1B - ESC键
    0x20 - 空格键
    0x08 - 退格键
    0x09 - Tab键
    0x21 - Page Up
    0x22 - Page Down
    0x23 - End
    0x24 - Home
    0x25 - 左箭头
    0x26 - 上箭头
    0x27 - 右箭头
    0x28 - 下箭头
    0x70-0x83 - F1-F24键
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
        button (int): 按钮代码 (1=左键, 2=右键, 4=中键, 8=X键)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def MouseUp(button):
    """
    鼠标抬起
    
    Args:
        button (int): 按钮代码 (1=左键, 2=右键, 4=中键, 8=X键)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def MouseClick(button):
    """
    鼠标点击（按下然后抬起）
    
    Args:
        button (int): 按钮代码 (1=左键, 2=右键, 4=中键, 8=X键)
    
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
        vk (int): 虚拟键码 (VK code)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeyUp(vk):
    """
    键盘抬起
    
    Args:
        vk (int): 虚拟键码 (VK code)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeyPress(vk):
    """
    键盘按下和抬起
    
    Args:
        vk (int): 虚拟键码 (VK code)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeyCombo(keys):
    """
    组合键
    
    Args:
        keys (list): 虚拟键码列表 (VK codes)
    
    Returns:
        bool: 操作是否成功
    """
    pass

def KeySeq(keys):
    """
    键盘序列
    
    Args:
        keys (list): 虚拟键码列表 (VK codes)
    
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
