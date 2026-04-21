"""
鼠标控制示例

演示如何使用 hid_controller 控制鼠标。
使用前请确保已安装并启动 Logitech Gaming Software (LGS)。

新特性：支持字符串方式调用鼠标函数！
    hid_controller.mouse_click("left")    # 推荐方式，简单易记
    hid_controller.mouse_press("right")
    hid_controller.mouse_down("middle")
    hid_controller.mouse_up("xbutton1")
"""

import time

import hid_controller


def main():
    print("=== HIDController 鼠标控制示例 ===\n")

    # 1. 相对移动
    print("1. 相对移动：鼠标向右移动 100 像素，向下移动 50 像素")
    hid_controller.move_mouse_relative(100, 50)
    time.sleep(0.5)

    # 2. 鼠标左键单击（使用字符串）
    print("2. 鼠标左键单击（字符串方式）")
    hid_controller.mouse_click("left")
    time.sleep(0.5)

    # 3. 鼠标右键单击（使用字符串）
    print("3. 鼠标右键单击（字符串方式）")
    hid_controller.mouse_click("right")
    time.sleep(0.5)

    # 4. 鼠标中键单击（使用字符串）
    print("4. 鼠标中键单击（字符串方式）")
    hid_controller.mouse_click("middle")
    time.sleep(0.5)

    # 5. 分步按下和释放（使用字符串）
    print("5. 分步操作：按下左键，然后释放（字符串方式）")
    hid_controller.mouse_down("left")
    time.sleep(0.3)
    hid_controller.mouse_up("left")
    time.sleep(0.5)

    # 6. 使用 mouse_press（类似 key_press，一键完成按下和释放）
    print("6. 使用 mouse_press 左键（字符串方式）")
    hid_controller.mouse_press("left")
    time.sleep(0.5)

    # 7. 使用 MouseEvent 常量（传统方式，仍然支持）
    print("7. 使用 MouseEvent 常量（传统方式）")
    hid_controller.mouse_click(hid_controller.MouseEvent.RIGHT)
    time.sleep(0.5)

    # 8. 使用数字常量（传统方式，仍然支持）
    print("8. 使用数字常量（传统方式）")
    hid_controller.mouse_click(0x02)  # MOUSEEVENTF_LEFTDOWN
    time.sleep(0.5)

    # 9. 鼠标滚轮
    print("9. 鼠标滚轮向上滚动一格")
    hid_controller.mouse_wheel(120)
    time.sleep(0.5)
    print("10. 鼠标滚轮向下滚动两格")
    hid_controller.mouse_wheel(-240)
    time.sleep(0.5)

    # 11. 禁用鼠标加速
    print("11. 禁用 Windows 鼠标加速")
    hid_controller.disable_mouse_acceleration()

    # 12. 设置鼠标速度系数
    print("12. 设置鼠标速度系数为 1.5")
    hid_controller.set_mouse_move_coefficient(1.5)

    # 13. 相对移动（使用系数后）
    print("13. 相对移动：鼠标向右移动 200 像素（使用系数 1.5）")
    hid_controller.move_mouse_relative(200, 0)
    time.sleep(0.5)

    # 14. 恢复鼠标设置
    print("14. 恢复 Windows 鼠标设置")
    hid_controller.enable_mouse_acceleration()

    print("\n=== 示例完成 ===")


if __name__ == "__main__":
    main()
