"""
高级用法示例

演示如何使用 hid_controller 实现更复杂的键盘鼠标控制。
使用前请确保已安装并启动 Logitech Gaming Software (LGS)。
"""

import hid_controller
import time


def draw_square(size: int = 100, delay: float = 0.1):
    """绘制正方形"""
    print(f"绘制边长为 {size} 的正方形")
    for _ in range(4):
        hid_controller.MouseMoveRelative(size, 0)
        time.sleep(delay)
        hid_controller.MouseMoveRelative(0, size)
    print("正方形绘制完成")


def type_text(text: str, delay: float = 0.05):
    """逐字符输入文本"""
    print(f"输入文本: {text}")
    for char in text:
        if char == ' ':
            hid_controller.KeyPress('space')
        elif char == '\n':
            hid_controller.KeyPress('enter')
        else:
            hid_controller.KeyPress(char)
        time.sleep(delay)
    print("文本输入完成")


def select_all():
    """全选 (Ctrl+A)"""
    hid_controller.KeyCombo(['lctrl', 'a'])
    time.sleep(0.1)


def copy():
    """复制 (Ctrl+C)"""
    hid_controller.KeyCombo(['lctrl', 'c'])
    time.sleep(0.1)


def paste():
    """粘贴 (Ctrl+V)"""
    hid_controller.KeyCombo(['lctrl', 'v'])
    time.sleep(0.1)


def main():
    print("=== HIDController 高级用法示例 ===\n")

    # 1. 绘制图形
    print("--- 绘制正方形 ---")
    draw_square(50, 0.05)
    time.sleep(0.5)

    # 2. 文本输入
    print("\n--- 文本输入 ---")
    type_text("Hello", 0.1)
    time.sleep(0.5)

    # 3. 模拟复制粘贴
    print("\n--- 模拟复制粘贴 ---")
    print("全选 -> 复制 -> 粘贴")
    select_all()
    copy()
    paste()
    time.sleep(0.5)

    # 4. 快速连续点击
    print("\n--- 快速连续点击 ---")
    for i in range(5):
        hid_controller.MouseClick(0x02)  # 左键单击
        time.sleep(0.1)
    print("5 次点击完成")

    # 5. 自动校准
    print("\n--- 自动校准鼠标速度 ---")
    hid_controller.AutoCalibrate()

    # 6. 释放所有按键
    print("\n--- 释放所有按键 ---")
    hid_controller.release_all_keys()

    print("\n=== 示例完成 ===")


if __name__ == "__main__":
    main()
