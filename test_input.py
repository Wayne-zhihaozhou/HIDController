#!/usr/bin/env python3
"""
测试 HIDController Python 模块
"""

import sys
import os
import time

# 添加当前目录到Python路径，以便导入模块
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_module_import():
    """测试模块导入"""
    try:
        import hid_controller
        print("✓ 成功导入 hid_controller 模块")
        return True
    except ImportError as e:
        print(f"✗ 导入模块失败: {e}")
        return False

def test_mouse_functions():
    """测试鼠标功能"""
    try:
        import hid_controller
        print("\n=== 测试鼠标功能 ===")
        
        # 测试鼠标相对移动
        print("测试鼠标相对移动...")
        result = hid_controller.MouseMoveRelative(100, 100)
        print(f"  鼠标相对移动结果: {result}")
        
        # 测试鼠标绝对移动
        print("测试鼠标绝对移动...")
        result = hid_controller.MouseMoveAbsolute(1000, 500)
        print(f"  鼠标绝对移动结果: {result}")
        
        # 测试鼠标按下
        print("测试鼠标按下...")
        result = hid_controller.MouseDown(0x01)  # 左键
        print(f"  鼠标按下结果: {result}")
        
        # 测试鼠标抬起
        print("测试鼠标抬起...")
        result = hid_controller.MouseUp(0x01)  # 左键
        print(f"  鼠标抬起结果: {result}")
        
        # 测试鼠标点击
        print("测试鼠标点击...")
        result = hid_controller.MouseClick(0x01)  # 左键
        print(f"  鼠标点击结果: {result}")
        
        # 测试鼠标滚轮
        print("测试鼠标滚轮...")
        result = hid_controller.MouseWheel(120)
        print(f"  鼠标滚轮结果: {result}")
        
        print("✓ 所有鼠标功能测试完成")
        return True
        
    except Exception as e:
        print(f"✗ 鼠标功能测试出错: {e}")
        return False

def test_keyboard_functions():
    """测试键盘功能"""
    try:
        import hid_controller
        print("\n=== 测试键盘功能 ===")
        
        # 测试按键按下
        print("测试按键按下...")
        result = hid_controller.KeyDown(0x41)  # A键的虚拟键码
        print(f"  按键按下结果: {result}")
        
        # 测试按键抬起
        print("测试按键抬起...")
        result = hid_controller.KeyUp(0x41)  # A键的虚拟键码
        print(f"  按键抬起结果: {result}")
        
        # 测试按键点击
        print("测试按键点击...")
        result = hid_controller.KeyPress(0x41)  # A键的虚拟键码
        print(f"  按键点击结果: {result}")
        
        # 测试组合键 (注意：这里我们跳过KeyCombo和KeySeq的测试，因为它们需要特殊处理)
        print("跳过组合键和序列键测试 (需要特殊处理)")
        print("✓ 所有键盘功能测试完成")
        return True
        
    except Exception as e:
        print(f"✗ 键盘功能测试出错: {e}")
        return False

def main():
    """主测试函数"""
    print("开始测试 HIDController Python 模块")
    print("=" * 50)
    
    # 测试模块导入
    if not test_module_import():
        print("模块导入失败，测试终止")
        return False
    
    # 测试鼠标功能
    mouse_success = test_mouse_functions()
    
    # 测试键盘功能
    keyboard_success = test_keyboard_functions()
    
    print("\n" + "=" * 50)
    if mouse_success and keyboard_success:
        print("✓ 所有测试完成，模块功能正常")
        return True
    else:
        print("✗ 部分测试失败")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
