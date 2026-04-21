"""
键盘控制示例

演示如何使用 hid_controller 控制键盘。
使用前请确保已安装并启动 Logitech Gaming Software (LGS)。

三种调用方式：
    1. 字符串方式（推荐）：hid_controller.key_press("Enter")
    2. KeyEvent 常量方式：hid_controller.key_press(KeyEvent.ENTER)
    3. 数字方式：hid_controller.key_press(13)
"""

import time

import hid_controller


def main():
    print("=== HIDController 键盘控制示例 ===\n")

    # ========== 方式1：字符串方式（推荐，简单易记）==========
    print("=== 方式1：字符串方式 ===")

    print("1. 按下 'a' 键（字符串）")
    hid_controller.key_press("a")
    time.sleep(0.3)

    print("2. 按下 Enter 键（字符串）")
    hid_controller.key_press("Enter")
    time.sleep(0.3)

    print("3. 组合键：Ctrl+C（字符串）")
    hid_controller.key_combo(["lctrl", "c"])
    time.sleep(0.5)

    print("4. 功能键 F1（字符串）")
    hid_controller.key_press("f1")
    time.sleep(0.5)

    print("5. 空格键（字符串）")
    hid_controller.key_press("space")
    time.sleep(0.3)

    # ========== 方式2：KeyEvent 常量方式（编辑器自动提示）==========
    print("\n=== 方式2：KeyEvent 常量方式 ===")

    print("6. 按下 'B' 键（KeyEvent 常量）")
    hid_controller.key_press(hid_controller.KeyEvent.B)
    time.sleep(0.3)

    print("7. 按下 Enter 键（KeyEvent.ENTER）")
    hid_controller.key_press(hid_controller.KeyEvent.RETURN)
    time.sleep(0.3)

    print("8. 按下左 Ctrl（KeyEvent.LCONTROL）")
    hid_controller.key_down(hid_controller.KeyEvent.LCONTROL)
    time.sleep(0.3)
    hid_controller.key_up(hid_controller.KeyEvent.LCONTROL)
    time.sleep(0.3)

    print("9. 组合键：Ctrl+A（KeyEvent 常量）")
    hid_controller.key_combo([hid_controller.KeyEvent.LCONTROL, hid_controller.KeyEvent.A])
    time.sleep(0.5)

    print("10. 方向键：上（KeyEvent.UP）")
    hid_controller.key_press(hid_controller.KeyEvent.UP)
    time.sleep(0.3)

    # ========== 方式3：数字方式（传统方式，仍然支持）==========
    print("\n=== 方式3：数字方式 ===")

    print("11. 按下 'C' 键（数字 0x43）")
    hid_controller.key_press(0x43)
    time.sleep(0.3)

    print("12. 按下 Enter 键（数字 13）")
    hid_controller.key_press(13)
    time.sleep(0.3)

    # ========== 按键序列 ==========
    print("\n=== 按键序列 ===")

    print("13. 按键序列：abc（字符串方式）")
    hid_controller.key_seq(["a", "b", "c"])
    time.sleep(0.5)

    print("14. 按键序列：123（KeyEvent 常量方式）")
    hid_controller.key_seq([
        hid_controller.KeyEvent.KEY_1,
        hid_controller.KeyEvent.KEY_2,
        hid_controller.KeyEvent.KEY_3
    ])
    time.sleep(0.5)

    # ========== 释放所有按键 ==========
    print("15. 释放所有按键")
    hid_controller.release_all_keys()

    print("\n=== 示例完成 ===")


if __name__ == "__main__":
    main()
