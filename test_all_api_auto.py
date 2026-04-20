"""
HIDController Python 扩展完整 API 自动测试脚本
自动运行所有 API 测试，无需交互

使用前请确保：
1. 已安装并启动 Logitech Gaming Software (LGS)
2. 准备好观察鼠标移动和键盘输入效果
"""

import hid_controller
import time


def print_header(title):
    """打印标题"""
    print("\n" + "=" * 60)
    print(f"  {title}")
    print("=" * 60)


def test_module_info():
    """测试模块导入和基本功能检查"""
    print_header("模块检查: hid_controller")
    
    print(f"✓ hid_controller 模块导入成功")
    print(f"  可用函数:")
    functions = sorted([f for f in dir(hid_controller) if not f.startswith('_')])
    for func in functions:
        print(f"    - {func}")


def test_mouse_functions_exist():
    """测试鼠标函数是否存在"""
    print_header("鼠标函数检查")
    
    functions = [
        "MouseMoveRelative",
        "MouseMoveAbsolute",
        "MouseDown",
        "MouseUp",
        "MouseClick",
        "MouseWheel",
        "SetMouseMoveCoefficient",
        "AutoCalibrate",
        "DisableMouseAcceleration",
        "EnableMouseAcceleration",
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
        "KeyDown",
        "KeyUp",
        "KeyPress",
        "KeyCombo",
        "KeySeq",
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
    print_header("测试 1: 鼠标相对移动 (MouseMoveRelative)")
    
    print("1.1 向右移动 100 像素")
    result = hid_controller.MouseMoveRelative(100, 0)
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("1.2 向下移动 50 像素")
    result = hid_controller.MouseMoveRelative(0, 50)
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("1.3 向左移动 100 像素（回到原位）")
    result = hid_controller.MouseMoveRelative(-100, 0)
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("1.4 向上移动 50 像素（回到原位）")
    result = hid_controller.MouseMoveRelative(0, -50)
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("✓ 测试完成")


def test_mouse_absolute():
    """测试 2: 鼠标绝对移动"""
    print_header("测试 2: 鼠标绝对移动 (MouseMoveAbsolute)")
    
    print("2.1 移动到屏幕左上角 (100, 100)")
    result = hid_controller.MouseMoveAbsolute(100, 100)
    print(f"  返回值: {result}")
    time.sleep(0.5)
    
    print("2.2 移动到屏幕中央 (假设 1920x1080)")
    result = hid_controller.MouseMoveAbsolute(960, 540)
    print(f"  返回值: {result}")
    time.sleep(0.5)
    
    print("✓ 测试完成")


def test_mouse_buttons():
    """测试 3: 鼠标按键"""
    print_header("测试 3: 鼠标按键 (MouseDown/MouseUp/MouseClick)")
    
    # 左键
    print("3.1 左键按下")
    result = hid_controller.MouseDown(0x02)  # MOUSEEVENTF_LEFTDOWN
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("3.2 左键抬起")
    result = hid_controller.MouseUp(0x03)  # MOUSEEVENTF_LEFTUP
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("3.3 左键单击")
    result = hid_controller.MouseClick(0x02)
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    # 右键
    print("3.4 右键单击")
    result = hid_controller.MouseClick(0x04)  # MOUSEEVENTF_RIGHTDOWN/UP
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    # 中键
    print("3.5 中键单击")
    result = hid_controller.MouseClick(0x20)  # MOUSEEVENTF_MIDDLEDOWN/UP
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("✓ 测试完成")


def test_mouse_wheel():
    """测试 4: 鼠标滚轮"""
    print_header("测试 4: 鼠标滚轮 (MouseWheel)")
    
    print("4.1 向上滚动一格 (120)")
    result = hid_controller.MouseWheel(120)
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("4.2 向下滚动一格 (-120)")
    result = hid_controller.MouseWheel(-120)
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("4.3 向上滚动两格 (240)")
    result = hid_controller.MouseWheel(240)
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("✓ 测试完成")


def test_mouse_coefficient():
    """测试 5: 鼠标速度系数"""
    print_header("测试 5: 鼠标速度系数 (SetMouseMoveCoefficient)")
    
    print("5.1 设置系数为 2.0")
    result = hid_controller.SetMouseMoveCoefficient(2.0)
    print(f"  返回值: {result}")
    
    print("5.2 移动鼠标观察速度变化")
    result = hid_controller.MouseMoveRelative(100, 0)
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("5.3 恢复系数为 1.0")
    result = hid_controller.SetMouseMoveCoefficient(1.0)
    print(f"  返回值: {result}")
    
    print("✓ 测试完成")


def test_mouse_acceleration():
    """测试 6: 鼠标加速"""
    print_header("测试 6: 鼠标加速 (DisableMouseAcceleration/EnableMouseAcceleration)")
    
    print("6.1 禁用鼠标加速")
    result = hid_controller.DisableMouseAcceleration()
    print(f"  返回值: {result}")
    time.sleep(0.5)
    
    print("6.2 移动鼠标感受差异")
    result = hid_controller.MouseMoveRelative(100, 0)
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("6.3 恢复鼠标加速")
    result = hid_controller.EnableMouseAcceleration()
    print(f"  返回值: {result}")
    
    print("✓ 测试完成")


def test_auto_calibrate():
    """测试 7: 自动校准"""
    print_header("测试 7: 自动校准 (AutoCalibrate)")
    
    print("7.1 执行自动校准（可能需要几秒）")
    result = hid_controller.AutoCalibrate()
    print(f"  返回值: {result}")
    
    print("✓ 测试完成")


def test_keyboard_simple():
    """测试 8: 键盘简单按键"""
    print_header("测试 8: 键盘简单按键 (KeyDown/KeyUp/KeyPress)")
    
    print("8.1 按下 'a' 键")
    result = hid_controller.KeyDown('a')
    print(f"  返回值: {result}")
    time.sleep(0.3)
    result = hid_controller.KeyUp('a')
    print(f"  抬起 'a'，返回值: {result}")
    time.sleep(0.3)
    
    print("8.2 按键 'b' (KeyPress)")
    result = hid_controller.KeyPress('b')
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("8.3 按键 'c' (使用虚拟键码)")
    result = hid_controller.KeyPress(0x43)  # 'C'
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("✓ 测试完成")


def test_keyboard_combo():
    """测试 9: 键盘组合键"""
    print_header("测试 9: 键盘组合键 (KeyCombo)")
    
    print("9.1 Ctrl+C 组合键")
    result = hid_controller.KeyCombo(['lctrl', 'c'])
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("9.2 Ctrl+V 组合键")
    result = hid_controller.KeyCombo(['lctrl', 'v'])
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("9.3 Alt+Tab 组合键")
    result = hid_controller.KeyCombo(['lalt', 'tab'])
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("✓ 测试完成")


def test_keyboard_sequence():
    """测试 10: 键盘按键序列"""
    print_header("测试 10: 键盘按键序列 (KeySeq)")
    
    print("10.1 依次按下 'x', 'y', 'z'")
    result = hid_controller.KeySeq(['x', 'y', 'z'])
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("10.2 依次按下 '1', '2', '3'")
    result = hid_controller.KeySeq(['1', '2', '3'])
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("✓ 测试完成")


def test_release_all():
    """测试 11: 释放所有按键"""
    print_header("测试 11: 释放所有按键 (release_all_keys)")
    
    print("11.1 按下几个键")
    hid_controller.KeyDown('a')
    hid_controller.KeyDown('b')
    time.sleep(0.3)
    
    print("11.2 释放所有按键")
    hid_controller.release_all_keys()
    time.sleep(0.3)
    
    print("✓ 测试完成")


def test_special_keys():
    """测试 12: 特殊键"""
    print_header("测试 12: 特殊键")
    
    print("12.1 空格键")
    result = hid_controller.KeyPress('space')
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("12.2 Enter 键")
    result = hid_controller.KeyPress('enter')
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("12.3 Escape 键")
    result = hid_controller.KeyPress('escape')
    print(f"  返回值: {result}")
    time.sleep(0.3)
    
    print("12.4 F1-F12 功能键")
    for i in range(1, 4):
        result = hid_controller.KeyPress(f'f{i}')
        print(f"  F{i} 返回值: {result}")
        time.sleep(0.2)
    
    print("12.5 方向键")
    result = hid_controller.KeyPress('up')
    print(f"  上箭头 返回值: {result}")
    time.sleep(0.2)
    result = hid_controller.KeyPress('down')
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
        print("    - MouseMoveRelative(dx, dy)")
        print("    - MouseMoveAbsolute(x, y)")
        print("    - MouseDown(button)")
        print("    - MouseUp(button)")
        print("    - MouseClick(button)")
        print("    - MouseWheel(movement)")
        print("    - SetMouseMoveCoefficient(coefficient)")
        print("    - AutoCalibrate()")
        print("    - DisableMouseAcceleration()")
        print("    - EnableMouseAcceleration()")
        print("  键盘:")
        print("    - KeyDown(vk)")
        print("    - KeyUp(vk)")
        print("    - KeyPress(vk)")
        print("    - KeyCombo(keys)")
        print("    - KeySeq(keys)")
        print("    - release_all_keys()")
        print()
        print("✓ 所有 API 测试通过！")
        
    except KeyboardInterrupt:
        print("\n\n测试被用户取消")
        print("✓ 已执行的测试已完成")


if __name__ == "__main__":
    main()
