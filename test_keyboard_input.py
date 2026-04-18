#!/usr/bin/env python3
"""
测试键盘输入功能的脚本
测试空格键和字母a是否可以正常触发
"""

import hid_controller
import time

def test_space_key():
    """测试空格键是否可以触发"""
    print("测试空格键...")
    try:
        # 按下空格键
        result = hid_controller.KeyPress("space")
        print(f"空格键按下结果: {result}")
        
        # 等待一小段时间
        time.sleep(0.1)
        
        # 再次按下空格键（测试重复按下）
        result2 = hid_controller.KeyPress("space")
        print(f"空格键第二次按下结果: {result2}")
        
        return result and result2
    except Exception as e:
        print(f"测试空格键时出错: {e}")
        return False

def test_a_key():
    """测试字母a是否可以触发"""
    print("测试字母a...")
    try:
        # 按下字母a键
        result = hid_controller.KeyPress("a")
        print(f"字母a按下结果: {result}")
        
        # 等待一小段时间
        time.sleep(0.1)
        
        # 再次按下字母a键（测试重复按下）
        result2 = hid_controller.KeyPress("a")
        print(f"字母a第二次按下结果: {result2}")
        
        return result and result2
    except Exception as e:
        print(f"测试字母a时出错: {e}")
        return False

def test_a_key_uppercase():
    """测试大写字母A是否可以触发"""
    print("测试大写字母A...")
    try:
        # 按下大写字母A键
        result = hid_controller.KeyPress("A")
        print(f"大写字母A按下结果: {result}")
        
        return result
    except Exception as e:
        print(f"测试大写字母A时出错: {e}")
        return False

def test_key_down_up():
    """测试按键按下和抬起功能"""
    print("测试按键按下和抬起功能...")
    try:
        # 按下空格键 (使用VK_SPACE虚拟键码)
        result1 = hid_controller.KeyDown(0x20)
        print(f"空格键按下结果: {result1}")
        
        # 等待一小段时间
        time.sleep(0.1)
        
        # 抬起空格键
        result2 = hid_controller.KeyUp(0x20)
        print(f"空格键抬起结果: {result2}")
        
        # 按下字母a (使用字符'a')
        result3 = hid_controller.KeyDown('a')
        print(f"字母a按下结果: {result3}")
        
        # 等待一小段时间
        time.sleep(0.1)
        
        # 抬起字母a
        result4 = hid_controller.KeyUp('a')
        print(f"字母a抬起结果: {result4}")
        
        return result1 and result2 and result3 and result4
    except Exception as e:
        print(f"测试按键按下和抬起时出错: {e}")
        return False

def main():
    """主测试函数"""
    print("开始测试键盘输入功能...")
    print("=" * 50)
    
    # 测试空格键
    space_test_result = test_space_key()
    print()
    
    # 测试字母a
    a_test_result = test_a_key()
    print()
    
    # 测试大写字母A
    a_upper_test_result = test_a_key_uppercase()
    print()
    
    # 测试按键按下和抬起
    down_up_test_result = test_key_down_up()
    print()
    
    print("=" * 50)
    print("测试结果汇总:")
    print(f"空格键测试: {'通过' if space_test_result else '失败'}")
    print(f"字母a测试: {'通过' if a_test_result else '失败'}")
    print(f"大写字母A测试: {'通过' if a_upper_test_result else '失败'}")
    print(f"按键按下/抬起测试: {'通过' if down_up_test_result else '失败'}")
    
    all_passed = space_test_result and a_test_result and a_upper_test_result and down_up_test_result
    print(f"总体测试: {'全部通过' if all_passed else '存在失败'}")
    
    return all_passed

if __name__ == "__main__":
    main()
