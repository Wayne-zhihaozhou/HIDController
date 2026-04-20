"""
鼠标控制示例

演示如何使用 hid_controller 控制鼠标。
使用前请确保已安装并启动 Logitech Gaming Software (LGS)。
"""

import hid_controller
import time


def main():
    print("=== HIDController 鼠标控制示例 ===\n")

    # 1. 相对移动
    print("1. 相对移动：鼠标向右移动 100 像素，向下移动 50 像素")
    hid_controller.MouseMoveRelative(100, 50)
    time.sleep(0.5)

    # 2. 鼠标按键
    print("2. 鼠标左键单击")
    hid_controller.MouseClick(0x02)  # MOUSEEVENTF_LEFTDOWN = 0x02
    time.sleep(0.5)

    # 3. 鼠标滚轮
    print("3. 鼠标滚轮向上滚动一格")
    hid_controller.MouseWheel(120)
    time.sleep(0.5)
    print("4. 鼠标滚轮向下滚动两格")
    hid_controller.MouseWheel(-240)
    time.sleep(0.5)

    # 5. 禁用鼠标加速
    print("5. 禁用 Windows 鼠标加速")
    hid_controller.DisableMouseAcceleration()

    # 6. 设置鼠标速度系数
    print("6. 设置鼠标速度系数为 1.5")
    hid_controller.SetMouseMoveCoefficient(1.5)

    # 7. 相对移动（使用系数后）
    print("7. 相对移动：鼠标向右移动 200 像素（使用系数 1.5）")
    hid_controller.MouseMoveRelative(200, 0)
    time.sleep(0.5)

    # 8. 恢复鼠标设置
    print("8. 恢复 Windows 鼠标设置")
    hid_controller.EnableMouseAcceleration()

    print("\n=== 示例完成 ===")


if __name__ == "__main__":
    main()
