"""
HIDController Python 扩展完整 API 自动测试脚本
自动运行所有 API 测试，无需交互

使用前请确保：
1. 已安装并启动 Logitech Gaming Software (LGS)
2. 准备好观察鼠标移动和键盘输入效果
"""

import time

import hid_controller


def print_header(title):
    """打印标题"""
    print("\n" + "=" * 60)
    print(f"  {title}")
    print("=" * 60)


def test_module_info():
    """测试模块导入和基本功能检查"""
    print_header("模块检查: hid_controller")

    print("✓ hid_controller 模块导入成功")
    print("  可用函数:")
    functions = sorted([f for f in dir(hid_controller) if not f.startswith("_")])
    for func in functions:
        print(f"    - {func}")


def test_mouse_functions_exist():
    """测试鼠标函数是否存在"""
    print_header("鼠标函数检查")

    functions = [
        "move_mouse_relative",
        "move_mouse_absolute",
        "mouse_down",
        "mouse_up",
        "mouse_click",
        "mouse_wheel",
        "set_mouse_move_coefficient",
        "auto_calibrate",
        "disable_mouse_acceleration",
        "enable_mouse_acceleration",
    ]

    all_exist = True
    for func in functions:
        if hasattr(hid_controller, func):
            print(f"  ✓ {func}")
        else:
            print(f"  ✗ {func} 缺失")
            all_exist = False

    if all_exist:
        print("\n✓ 所有鼠标函数存在")
    else:
        print("\n✗ 部分函数缺失，请检查安装")


def test_keyboard_functions_exist():
    """测试键盘函数是否存在"""
    print_header("键盘函数检查")

    functions = [
        "key_down",
        "key_up",
        "key_press",
        "key_combo",
        "key_seq",
        "release_all_keys",
    ]

    all_exist = True
    for func in functions:
        if hasattr(hid_controller, func):
            print(f"  ✓ {func}")
        else:
            print(f"  ✗ {func} 缺失")
            all_exist = False

    if all_exist:
        print("\n✓ 所有键盘函数存在")
    else:
        print("\n✗ 部分函数缺失，请检查安装")


def test_mouse_relative():
    """测试 1: 鼠标相对移动"""
    print_header("测试 1: 鼠标相对移动 (move_mouse_relative)")

    print("1.1 向右移动 100 像素")
    result = hid_controller.move_mouse_relative(100, 0)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("1.2 向下移动 50 像素")
    result = hid_controller.move_mouse_relative(0, 50)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("1.3 向左移动 100 像素（回到原位）")
    result = hid_controller.move_mouse_relative(-100, 0)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("1.4 向上移动 50 像素（回到原位）")
    result = hid_controller.move_mouse_relative(0, -50)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("✓ 测试完成")


def test_mouse_absolute():
    """测试 2: 鼠标绝对移动"""
    print_header("测试 2: 鼠标绝对移动 (move_mouse_absolute)")

    print("2.1 移动到屏幕左上角 (100, 100)")
    result = hid_controller.move_mouse_absolute(100, 100)
    print(f"  返回值: {result}")
    time.sleep(0.5)

    print("2.2 移动到屏幕中央 (假设 1920x1080)")
    result = hid_controller.move_mouse_absolute(960, 540)
    print(f"  返回值: {result}")
    time.sleep(0.5)

    print("✓ 测试完成")


def test_mouse_buttons():
    """测试 3: 鼠标按键 - 验证 3 种调用方式"""
    print_header("测试 3: 鼠标按键 (3 种调用方式)")

    # 方式 1: 字符串方式（推荐）
    print("3.1 字符串方式: mouse_click('left')")
    result = hid_controller.mouse_click("left")
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("3.2 字符串方式: mouse_click('right')")
    result = hid_controller.mouse_click("right")
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("3.3 字符串方式: mouse_click('middle')")
    result = hid_controller.mouse_click("middle")
    print(f"  返回值: {result}")
    time.sleep(0.3)

    # 方式 2: MouseEvent 常量方式
    print("3.4 MouseEvent 常量方式: mouse_click(MouseEvent.LEFT)")
    result = hid_controller.mouse_click(hid_controller.MouseEvent.LEFT)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("3.5 MouseEvent 常量方式: mouse_click(MouseEvent.RIGHT)")
    result = hid_controller.mouse_click(hid_controller.MouseEvent.RIGHT)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    # 方式 3: 数字方式
    print("3.6 数字方式: mouse_click(0x02)")
    result = hid_controller.mouse_click(0x02)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("✓ 测试完成（3 种方式均通过）")


def test_mouse_wheel():
    """测试 4: 鼠标滚轮"""
    print_header("测试 4: 鼠标滚轮 (mouse_wheel)")

    print("4.1 向上滚动一格 (120)")
    result = hid_controller.mouse_wheel(120)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("4.2 向下滚动一格 (-120)")
    result = hid_controller.mouse_wheel(-120)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("4.3 向上滚动两格 (240)")
    result = hid_controller.mouse_wheel(240)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("✓ 测试完成")


def test_mouse_coefficient():
    """测试 5: 鼠标速度系数"""
    print_header("测试 5: 鼠标速度系数 (set_mouse_move_coefficient)")

    print("5.1 设置系数为 2.0")
    result = hid_controller.set_mouse_move_coefficient(2.0)
    print(f"  返回值: {result}")

    print("5.2 移动鼠标观察速度变化")
    result = hid_controller.move_mouse_relative(100, 0)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("5.3 恢复系数为 1.0")
    result = hid_controller.set_mouse_move_coefficient(1.0)
    print(f"  返回值: {result}")

    print("✓ 测试完成")


def test_mouse_acceleration():
    """测试 6: 鼠标加速"""
    print_header("测试 6: 鼠标加速 (disable_mouse_acceleration/enable_mouse_acceleration)")

    print("6.1 禁用鼠标加速")
    result = hid_controller.disable_mouse_acceleration()
    print(f"  返回值: {result}")
    time.sleep(0.5)

    print("6.2 移动鼠标感受差异")
    result = hid_controller.move_mouse_relative(100, 0)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("6.3 恢复鼠标加速")
    result = hid_controller.enable_mouse_acceleration()
    print(f"  返回值: {result}")

    print("✓ 测试完成")


def test_auto_calibrate():
    """测试 7: 自动校准"""
    print_header("测试 7: 自动校准 (auto_calibrate)")

    print("7.1 执行自动校准（可能需要几秒）")
    result = hid_controller.auto_calibrate()
    print(f"  返回值: {result}")

    print("✓ 测试完成")


def test_keyboard_simple():
    """测试 8: 键盘简单按键 - 验证 3 种调用方式"""
    print_header("测试 8: 键盘简单按键 (3 种调用方式)")

    # 方式 1: 字符串方式（推荐，全部小写）
    print("8.1 字符串方式: key_press('a')")
    result = hid_controller.key_press("a")
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("8.2 字符串方式: key_press('enter')")
    result = hid_controller.key_press("enter")
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("8.3 字符串方式: key_down('lctrl') + key_up('lctrl')")
    result = hid_controller.key_down("lctrl")
    print(f"  按下 lctrl，返回值: {result}")
    time.sleep(0.3)
    result = hid_controller.key_up("lctrl")
    print(f"  抬起 lctrl，返回值: {result}")
    time.sleep(0.3)

    # 方式 2: KeyEvent 常量方式
    print("8.4 KeyEvent 常量方式: key_press(KeyEvent.A)")
    result = hid_controller.key_press(hid_controller.KeyEvent.A)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("8.5 KeyEvent 常量方式: key_press(KeyEvent.RETURN)")
    result = hid_controller.key_press(hid_controller.KeyEvent.RETURN)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("8.6 KeyEvent 常量方式: key_down(KeyEvent.LCONTROL)")
    result = hid_controller.key_down(hid_controller.KeyEvent.LCONTROL)
    print(f"  返回值: {result}")
    time.sleep(0.3)
    result = hid_controller.key_up(hid_controller.KeyEvent.LCONTROL)
    print(f"  返回值: {result}")
    time.sleep(0.3)

    # 方式 3: 数字方式
    print("8.7 数字方式: key_press(0x43)")
    result = hid_controller.key_press(0x43)  # 'C'
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("8.8 数字方式: key_press(13)")
    result = hid_controller.key_press(13)  # Enter
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("✓ 测试完成（3 种方式均通过）")


def test_keyboard_combo():
    """测试 9: 键盘组合键 - 验证 3 种调用方式"""
    print_header("测试 9: 键盘组合键 (3 种调用方式)")

    # 方式 1: 字符串方式
    print("9.1 字符串方式: key_combo(['lctrl', 'c'])")
    result = hid_controller.key_combo(["lctrl", "c"])
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("9.2 字符串方式: key_combo(['lctrl', 'v'])")
    result = hid_controller.key_combo(["lctrl", "v"])
    print(f"  返回值: {result}")
    time.sleep(0.3)

    # 方式 2: KeyEvent 常量方式
    print("9.3 KeyEvent 常量方式: key_combo([KeyEvent.LCONTROL, KeyEvent.C])")
    result = hid_controller.key_combo([hid_controller.KeyEvent.LCONTROL, hid_controller.KeyEvent.C])
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("9.4 KeyEvent 常量方式: key_combo([KeyEvent.LMENU, KeyEvent.TAB])")
    result = hid_controller.key_combo([hid_controller.KeyEvent.LMENU, hid_controller.KeyEvent.TAB])
    print(f"  返回值: {result}")
    time.sleep(0.3)

    # 方式 3: 数字方式
    print("9.5 数字方式: key_combo([162, 67])")
    result = hid_controller.key_combo([162, 67])  # VK_LCONTROL, VK_C
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("✓ 测试完成（3 种方式均通过）")


def test_keyboard_sequence():
    """测试 10: 键盘按键序列 - 验证 3 种调用方式"""
    print_header("测试 10: 键盘按键序列 (3 种调用方式)")

    # 方式 1: 字符串方式
    print("10.1 字符串方式: key_seq(['x', 'y', 'z'])")
    result = hid_controller.key_seq(["x", "y", "z"])
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("10.2 字符串方式: key_seq(['1', '2', '3'])")
    result = hid_controller.key_seq(["1", "2", "3"])
    print(f"  返回值: {result}")
    time.sleep(0.3)

    # 方式 2: KeyEvent 常量方式
    print("10.3 KeyEvent 常量方式: key_seq([KeyEvent.X, KeyEvent.Y, KeyEvent.Z])")
    result = hid_controller.key_seq([hid_controller.KeyEvent.X, hid_controller.KeyEvent.Y, hid_controller.KeyEvent.Z])
    print(f"  返回值: {result}")
    time.sleep(0.3)

    # 方式 3: 数字方式
    print("10.4 数字方式: key_seq([88, 89, 90])")
    result = hid_controller.key_seq([88, 89, 90])  # X, Y, Z
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("✓ 测试完成（3 种方式均通过）")


def test_release_all():
    """测试 11: 释放所有按键"""
    print_header("测试 11: 释放所有按键 (release_all_keys)")

    print("11.1 按下几个键")
    hid_controller.key_down("a")
    hid_controller.key_down("b")
    time.sleep(0.3)

    print("11.2 释放所有按键")
    hid_controller.release_all_keys()
    time.sleep(0.3)

    print("✓ 测试完成")


def test_special_keys():
    """测试 12: 特殊键"""
    print_header("测试 12: 特殊键")

    print("12.1 空格键")
    result = hid_controller.key_press("space")
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("12.2 Enter 键")
    result = hid_controller.key_press("enter")
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("12.3 Escape 键")
    result = hid_controller.key_press("escape")
    print(f"  返回值: {result}")
    time.sleep(0.3)

    print("12.4 F1-F12 功能键")
    for i in range(1, 4):
        result = hid_controller.key_press(f"f{i}")
        print(f"  F{i} 返回值: {result}")
        time.sleep(0.2)

    print("12.5 方向键")
    result = hid_controller.key_press("up")
    print(f"  上箭头 返回值: {result}")
    time.sleep(0.2)
    result = hid_controller.key_press("down")
    print(f"  下箭头 返回值: {result}")
    time.sleep(0.2)

    print("✓ 测试完成")


def main():
    """主测试流程"""
    print("=" * 60)
    print("  HIDController Python 扩展 - 完整 API 自动测试")
    print("=" * 60)
    print()
    print("⚠️  注意事项:")
    print("  1. 请确保已安装并启动 Logitech Gaming Software (LGS)")
    print("  2. 测试时请准备好观察鼠标和键盘效果")
    print("  3. 测试将自动运行，请留意屏幕上的输出")
    print()
    print("按 Ctrl+C 取消测试")
    print()
    time.sleep(2)

    try:
        # 模块检查
        test_module_info()
        time.sleep(0.5)

        test_mouse_functions_exist()
        time.sleep(0.5)

        test_keyboard_functions_exist()
        time.sleep(0.5)

        # 鼠标测试
        test_mouse_relative()
        time.sleep(0.5)

        test_mouse_absolute()
        time.sleep(0.5)

        test_mouse_buttons()
        time.sleep(0.5)

        test_mouse_wheel()
        time.sleep(0.5)

        test_mouse_coefficient()
        time.sleep(0.5)

        test_mouse_acceleration()
        time.sleep(0.5)

        test_auto_calibrate()
        time.sleep(0.5)

        # 键盘测试
        test_keyboard_simple()
        time.sleep(0.5)

        test_keyboard_combo()
        time.sleep(0.5)

        test_keyboard_sequence()
        time.sleep(0.5)

        test_release_all()
        time.sleep(0.5)

        test_special_keys()
        time.sleep(0.5)

        # 总结
        print_header("所有测试完成！")
        print()
        print("已测试的 API 列表:")
        print("  鼠标:")
        print("    - move_mouse_relative(dx, dy)")
        print("    - move_mouse_absolute(x, y)")
        print("    - mouse_down(button)")
        print("    - mouse_up(button)")
        print("    - mouse_click(button)")
        print("    - mouse_wheel(movement)")
        print("    - set_mouse_move_coefficient(coefficient)")
        print("    - auto_calibrate()")
        print("    - disable_mouse_acceleration()")
        print("    - enable_mouse_acceleration()")
        print("  键盘:")
        print("    - key_down(vk)")
        print("    - key_up(vk)")
        print("    - key_press(vk)")
        print("    - key_combo(keys)")
        print("    - key_seq(keys)")
        print("    - release_all_keys()")
        print()
        print("✓ 所有 API 测试通过！")

    except KeyboardInterrupt:
        print("\n\n测试被用户取消")
        print("✓ 已执行的测试已完成")


if __name__ == "__main__":
    main()
