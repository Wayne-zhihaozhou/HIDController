"""
键盘控制示例

演示如何使用 hid_controller 控制键盘。
使用前请确保已安装并启动 Logitech Gaming Software (LGS)。
"""

import hid_controller
import time


def main():
    print("=== HIDController 键盘控制示例 ===\n")

    # 1. 单键按下（使用字符串键名）
    print("1. 按下 'a' 键")
    hid_controller.KeyDown('a')
    time.sleep(0.3)
    hid_controller.KeyUp('a')
    time.sleep(0.3)

    # 2. 按键（按下并抬起）
    print("2. 按下 'b' 键")
    hid_controller.KeyPress('b')
    time.sleep(0.3)

    # 3. 使用虚拟键码
    print("3. 按下 'c' 键（使用虚拟键码）")
    hid_controller.KeyPress(0x43)  # 'C' 的虚拟键码
    time.sleep(0.3)

    # 4. 组合键（Ctrl+C）
    print("4. 组合键：Ctrl+C")
    hid_controller.KeyCombo(['lctrl', 'c'])
    time.sleep(0.5)

    # 5. 组合键（Ctrl+A）
    print("5. 组合键：Ctrl+A")
    hid_controller.KeyCombo(['lctrl', 'a'])
    time.sleep(0.5)

    # 6. 按键序列（依次按下并释放）
    print("6. 按键序列：abc")
    hid_controller.KeySeq(['a', 'b', 'c'])
    time.sleep(0.5)

    # 7. 功能键
    print("7. 按下 F1 键")
    hid_controller.KeyPress('f1')
    time.sleep(0.5)

    # 8. 特殊键
    print("8. 按下空格键")
    hid_controller.KeyPress('space')
    time.sleep(0.3)

    # 9. 释放所有按键
    print("9. 释放所有按键")
    hid_controller.release_all_keys()

    print("\n=== 示例完成 ===")


if __name__ == "__main__":
    main()
