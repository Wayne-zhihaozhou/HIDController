"""
HIDController Input Tracker - 键盘鼠标事件检测模块

基于 Windows RAW INPUT API 实时检测键盘鼠标事件。

使用方式：
    1. 回调模式（事件驱动）：
        import hid_controller.input_tracker as tracker
        
        def on_mouse_move(device_handle, dx, dy):
            print(f"Mouse moved: dx={dx}, dy={dy}")
        
        def on_key_event(device_handle, vkey, is_down):
            state = "DOWN" if is_down else "UP"
            print(f"Key {state}: vkey={vkey}")
        
        def on_mouse_button(device_handle, button, is_down):
            btn_names = {1: "Left", 2: "Right", 3: "Middle"}
            print(f"Mouse {btn_names.get(button, button)} button: {'DOWN' if is_down else 'UP'}")
        
        tracker.start(on_mouse_move, on_key_event, on_mouse_button)
        # ... 运行你的代码 ...
        tracker.stop()
    
    2. 轮询模式（主动查询）：
        import hid_controller.input_tracker as tracker
        
        tracker.start()  # 不传回调函数
        # ... 运行你的代码 ...
        dx, dy = tracker.get_mouse_delta()
        keys = tracker.get_pressed_keys()
        tracker.stop()

API 参考：
    - start(mouse_callback, key_callback, mouse_button_callback) - 开始跟踪
    - stop() - 停止跟踪
    - is_tracking() - 检查是否正在跟踪
    - get_mouse_delta() - 获取鼠标增量（轮询模式）
    - get_pressed_keys() - 获取当前按下的键（轮询模式）

回调函数签名：
    - mouse_callback(device_handle: int, dx: int, dy: int) -> None
    - key_callback(device_handle: int, vkey: int, is_down: bool) -> None
    - mouse_button_callback(device_handle: int, button: int, is_down: bool) -> None
    
    button 值: 1=左键, 2=右键, 3=中键
"""

import sys
import os

# 尝试导入 C++ 扩展模块
try:
    from ._input_tracker import (
        start as _start,
        stop as _stop,
        is_tracking as _is_tracking,
        get_mouse_delta as _get_mouse_delta,
        get_pressed_keys as _get_pressed_keys,
    )
except ImportError as e:
    # 如果导入失败，提供友好的错误信息
    _import_error = str(e)
    
    def _raise_error(*args, **kwargs):
        raise RuntimeError(
            f"input_tracker 扩展模块未编译或导入失败。\n"
            f"错误: {_import_error}\n"
            f"请运行: python setup.py build_ext --inplace\n"
            f"或: pip install -e ."
        )
    
    _start = _raise_error
    _stop = _raise_error
    _is_tracking = lambda: False
    _get_mouse_delta = lambda: (0, 0)
    _get_pressed_keys = lambda: []


def start(mouse_callback=None, key_callback=None, mouse_button_callback=None):
    """
    开始跟踪键盘和鼠标事件。
    
    参数:
        mouse_callback: 鼠标移动回调函数，签名: (device_handle, dx, dy) -> None
                       device_handle: 设备句柄（uintptr_t）
                       dx: X 轴位移
                       dy: Y 轴位移
        key_callback: 键盘回调函数，签名: (device_handle, vkey, is_down) -> None
                     vkey: 虚拟键码 (VK_*)
                     is_down: True=按下, False=抬起
        mouse_button_callback: 鼠标按键回调函数，签名: (device_handle, button, is_down) -> None
                              button: 1=左键, 2=右键, 3=中键
    
    示例:
        >>> def on_mouse(dx, dy):
        ...     print(f"Moved: {dx}, {dy}")
        >>> start(mouse_callback=on_mouse)
    """
    _start(mouse_callback or None, key_callback or None, mouse_button_callback or None)


def stop():
    """停止跟踪键盘和鼠标事件。"""
    _stop()


def is_tracking():
    """
    检查是否正在跟踪输入事件。
    
    返回:
        bool: True=正在跟踪, False=未跟踪
    """
    return _is_tracking()


def get_mouse_delta():
    """
    获取自上次调用以来的累积鼠标位移（轮询模式）。
    
    返回:
        tuple: (dx, dy) - 鼠标位移量
    """
    return _get_mouse_delta()


def get_pressed_keys():
    """
    获取当前所有按下的虚拟键码列表（轮询模式）。
    
    返回:
        list: 虚拟键码列表
    """
    return list(_get_pressed_keys())


# 模块文档
__doc__ = """
HIDController Input Tracker - 键盘鼠标事件检测模块

基于 Windows RAW INPUT API 实时检测键盘鼠标事件。

使用方式：
    1. 回调模式（事件驱动）
    2. 轮询模式（主动查询）

API:
    - start(mouse_callback, key_callback, mouse_button_callback) - 开始跟踪
    - stop() - 停止跟踪
    - is_tracking() - 检查是否正在跟踪
    - get_mouse_delta() - 获取鼠标增量
    - get_pressed_keys() - 获取当前按下的键
"""

__all__ = [
    "start",
    "stop", 
    "is_tracking",
    "get_mouse_delta",
    "get_pressed_keys",
]
