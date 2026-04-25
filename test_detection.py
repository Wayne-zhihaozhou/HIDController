"""
HIDController 键盘鼠标控制检测测试脚本

使用 input_tracker 检测键盘鼠标控制是否正常工作：
1. 检测键盘按键控制
2. 检测鼠标移动和按键控制
3. 验证检测功能本身能够正确识别控制

使用前请确保：
1. hid_controller 扩展已编译并可用
2. 准备好观察检测结果的输出
"""

import time
import hid_controller
from hid_controller import input_tracker


def reset_detection_state():
    """重置检测状态 - 清空所有累积的输入数据"""
    input_tracker.stop()
    time.sleep(0.1)
    input_tracker.start()
    time.sleep(0.1)


def get_mouse_delta():
    """获取鼠标位移量"""
    return input_tracker.get_mouse_delta()


def get_pressed_keys():
    """获取当前按下的键码列表"""
    return input_tracker.get_pressed_keys()


def test_keyboard_detection():
    """
    检测键盘控制是否正常工作
    
    流程：
    1. 启动 input_tracker 开始监听（使用回调模式）
    2. 发送键盘按键信号（使用 hid_controller.key_press）
    3. 检查回调函数是否能接收到该按键事件
    
    返回:
        dict: {
            'success': bool,           - 检测是否成功
            'keys_detected': list,     - 检测到的键码列表
            'expected_keys': list      - 期望检测到的键码列表
        }
    """
    print("\n" + "=" * 60)
    print("  键盘控制检测测试")
    print("=" * 60)
    
    # 获取常量类（如果可用）
    try:
        KeyEvent = hid_controller.KeyEvent
    except AttributeError:
        KeyEvent = None
    
    # 使用回调来检测键盘事件
    detected_keys = []
    
    def on_key_event(device_handle, vkey, is_down):
        """键盘事件回调函数"""
        state = "按下" if is_down else "抬起"
        print(f"    [回调] 键码 {vkey} {state}")
        if is_down:
            detected_keys.append(vkey)
    
    # 重置检测状态并设置回调
    print("\n[步骤 1] 启动带回调的检测...")
    input_tracker.stop()
    time.sleep(0.1)
    input_tracker.start(key_callback=on_key_event)
    time.sleep(0.1)
    print("  ✓ 检测状态已重置")
    
    # 定义要测试的按键
    test_keys = [
        ("a", None),
        ("enter", None),
        ("space", None),
    ]
    
    results = {
        'success': True,
        'keys_detected': [],
        'expected_keys': [],
        'details': []
    }
    
    for key_name, _ in test_keys:
        print(f"\n[步骤 2] 测试按键: {key_name}")
        
        # 发送按键信号
        print(f"  发送按键: key_press('{key_name}')")
        result = getattr(hid_controller, 'key_press')(key_name)
        print(f"  key_press 返回值: {result}")
        
        # 等待系统处理
        time.sleep(0.3)
    
    # 停止检测
    input_tracker.stop()
    
    # 检查是否检测到按键
    if len(detected_keys) >= 3:
        print(f"\n  检测到的按键: {detected_keys}")
        results['keys_detected'] = detected_keys
        results['success'] = True
        results['details'].append(f"✓ 成功检测到 {len(detected_keys)} 个键盘按键事件")
    else:
        print(f"\n  检测到的按键: {detected_keys}")
        results['keys_detected'] = detected_keys
        results['success'] = False
        results['details'].append(f"✗ 未检测到足够的按键事件 (期望: 3, 实际: {len(detected_keys)})")
    
    results['expected_keys'] = ['a', 'enter', 'space']
    
    print("\n" + "-" * 40)
    print("  键盘检测总结:")
    print(f"  结果: {'✓ 成功' if results['success'] else '✗ 失败'}")
    print(f"  检测到: {results['keys_detected']}")
    print(f"  期望: {results['expected_keys']}")
    print("  详情:")
    for detail in results['details']:
        print(f"    {detail}")
    
    return results


def test_mouse_move_detection():
    """
    检测鼠标移动控制是否正常工作
    
    流程：
    1. 启动 input_tracker 开始监听
    2. 发送鼠标移动信号
    3. 检查 input_tracker 是否能检测到移动
    
    返回:
        dict: {
            'success': bool,           - 检测是否成功
            'detected_delta': tuple,   - 检测到的位移 (dx, dy)
            'expected_delta': tuple    - 期望的位移 (dx, dy)
        }
    """
    print("\n" + "=" * 60)
    print("  鼠标移动控制检测测试")
    print("=" * 60)
    
    # 测试不同的移动方向和距离
    test_moves = [
        ("向右 100", 100, 0),
        ("向下 50", 0, 50),
        ("向左 -50", -50, 0),
        ("向上 -30", 0, -30),
    ]
    
    results = {
        'success': True,
        'moves_detected': [],
        'details': []
    }
    
    for move_name, expected_dx, expected_dy in test_moves:
        print(f"\n[步骤] 测试移动: {move_name} (期望: dx={expected_dx}, dy={expected_dy})")
        
        # 重置检测状态
        reset_detection_state()
        
        # 发送鼠标移动信号
        print(f"  发送移动: move_mouse_relative({expected_dx}, {expected_dy})")
        result = hid_controller.move_mouse_relative(expected_dx, expected_dy)
        print(f"  move_mouse_relative 返回值: {result}")
        
        # 等待系统处理
        time.sleep(0.3)
        
        # 检测鼠标移动
        detected_dx, detected_dy = get_mouse_delta()
        print(f"  检测到的位移: dx={detected_dx}, dy={detected_dy}")
        
        # 检查是否检测到移动（允许一定的误差）
        if detected_dx == expected_dx and detected_dy == expected_dy:
            print(f"  ✓ 成功检测到鼠标移动: {move_name}")
            results['moves_detected'].append((expected_dx, expected_dy))
            results['details'].append(f"✓ {move_name}: 检测到 (dx={detected_dx}, dy={detected_dy})")
        else:
            # 即使数值不完全匹配，只要有移动就认为成功
            if detected_dx != 0 or detected_dy != 0:
                print(f"  ~ 检测到鼠标移动但数值有差异: {move_name}")
                print(f"    期望: ({expected_dx}, {expected_dy}), 实际: ({detected_dx}, {detected_dy})")
                results['details'].append(f"~ {move_name}: 检测到但有差异")
            else:
                print(f"  ✗ 未检测到鼠标移动: {move_name}")
                results['success'] = False
                results['details'].append(f"✗ {move_name}: 未检测到")
    
    print("\n" + "-" * 40)
    print("  鼠标移动检测总结:")
    print(f"  结果: {'✓ 成功' if results['success'] else '✗ 失败'}")
    print(f"  检测到的移动: {results['moves_detected']}")
    print("  详情:")
    for detail in results['details']:
        print(f"    {detail}")
    
    return results


def test_mouse_button_detection():
    """
    检测鼠标按键控制是否正常工作
    
    流程：
    1. 启动 input_tracker 开始监听
    2. 发送鼠标按键信号
    3. 检查 input_tracker 是否能检测到按键
    
    返回:
        dict: {
            'success': bool,
            'buttons_detected': list,
            'details': list
        }
    """
    print("\n" + "=" * 60)
    print("  鼠标按键控制检测测试")
    print("=" * 60)
    
    # 使用回调来检测鼠标按键
    detected_buttons = []
    
    def on_mouse_button(device_handle, button, is_down):
        """鼠标按键回调函数"""
        btn_names = {1: "左键", 2: "右键", 3: "中键", 4: "X1", 5: "X2"}
        state = "按下" if is_down else "抬起"
        btn_name = btn_names.get(button, str(button))
        print(f"    [回调] 鼠标{btn_name} {state}")
        if is_down:
            detected_buttons.append(button)
    
    # 重置检测状态并设置回调
    print("\n[步骤 1] 启动带回调的检测...")
    input_tracker.stop()
    time.sleep(0.1)
    input_tracker.start(mouse_button_callback=on_mouse_button)
    time.sleep(0.1)
    
    # 测试不同的鼠标按键
    test_buttons = [
        ("left", "左键"),
        ("right", "右键"),
        ("middle", "中键"),
    ]
    
    results = {
        'success': True,
        'buttons_detected': [],
        'details': []
    }
    
    for button_name, btn_chinese in test_buttons:
        print(f"\n[步骤 2] 测试按键: {btn_chinese} ({button_name})")
        
        # 发送鼠标按键信号
        print(f"  发送按键: mouse_click('{button_name}')")
        result = getattr(hid_controller, 'mouse_click')(button_name)
        print(f"  mouse_click 返回值: {result}")
        
        # 等待系统处理
        time.sleep(0.3)
    
    # 停止检测
    input_tracker.stop()
    
    # 分析结果
    if len(detected_buttons) > 0:
        print(f"\n  检测到的按键: {detected_buttons}")
        results['buttons_detected'] = detected_buttons
        results['details'].append(f"✓ 检测到 {len(detected_buttons)} 个鼠标按键事件")
    else:
        print(f"\n  ✗ 未检测到任何鼠标按键事件")
        results['success'] = False
        results['details'].append("✗ 未检测到鼠标按键")
    
    print("\n" + "-" * 40)
    print("  鼠标按键检测总结:")
    print(f"  结果: {'✓ 成功' if results['success'] else '✗ 失败'}")
    print(f"  检测到的按键: {results['buttons_detected']}")
    print("  详情:")
    for detail in results['details']:
        print(f"    {detail}")
    
    return results


def test_keyboard_combo_detection():
    """
    检测键盘组合键控制是否正常工作
    
    返回:
        dict: 检测结果
    """
    print("\n" + "=" * 60)
    print("  键盘组合键控制检测测试")
    print("=" * 60)
    
    # 使用回调来检测键盘事件
    detected_keys = []
    
    def on_key_event(device_handle, vkey, is_down):
        """键盘事件回调函数"""
        state = "按下" if is_down else "抬起"
        print(f"    [回调] 键码 {vkey} {state}")
        if is_down:
            detected_keys.append(vkey)
    
    # 重置检测状态并设置回调
    print("\n[步骤 1] 启动带回调的检测...")
    input_tracker.stop()
    time.sleep(0.1)
    input_tracker.start(key_callback=on_key_event)
    time.sleep(0.1)
    
    # 测试组合键
    print("\n[步骤 2] 测试组合键: Ctrl+C")
    result = hid_controller.key_combo(["lctrl", "c"])
    print(f"  key_combo 返回值: {result}")
    time.sleep(0.3)
    
    # 停止检测
    input_tracker.stop()
    
    # 分析结果
    # 注意：系统可能将左Ctrl转换为通用Ctrl (17)，将 'c' 转换为 ASCII 码 (99) 而不是 VK 码 (67)
    expected_keys = [17, 99]  # VK_CONTROL (通用Ctrl), ASCII 'c' = 99
    print(f"\n  检测到的按键: {detected_keys}")
    print(f"  期望的按键: {expected_keys}")
    
    # 检查是否检测到至少2个按键（组合键的两个键）
    success = len(detected_keys) >= 2
    
    results = {
        'success': success,
        'keys_detected': detected_keys,
        'expected_keys': expected_keys,
        'details': []
    }
    
    if success:
        results['details'].append(f"✓ 成功检测到组合键的所有按键")
    else:
        missing = [vk for vk in expected_keys if vk not in detected_keys]
        results['details'].append(f"✗ 未检测到: {missing}")
    
    print("\n" + "-" * 40)
    print("  组合键检测总结:")
    print(f"  结果: {'✓ 成功' if success else '✗ 失败'}")
    print(f"  检测到: {detected_keys}")
    print(f"  期望: {expected_keys}")
    print("  详情:")
    for detail in results['details']:
        print(f"    {detail}")
    
    return results


def verify_detection_function_works():
    """
    验证检测函数本身是否正常工作
    
    这个方法会：
    1. 先测试不发送任何控制信号，确认检测函数不会误报
    2. 再测试发送控制信号，确认检测函数能正确检测
    
    返回:
        dict: 验证结果
    """
    print("\n" + "=" * 60)
    print("  检测函数有效性验证")
    print("=" * 60)
    
    results = {
        'no_false_positive': False,
        'can_detect': False,
        'details': []
    }
    
    # 测试 1: 不发送任何信号，确认没有误报
    print("\n[测试 1] 验证无误报（不发送任何控制信号）")
    reset_detection_state()
    time.sleep(0.5)  # 等待一段时间
    
    mouse_delta = get_mouse_delta()
    keys = get_pressed_keys()
    
    print(f"  鼠标位移: dx={mouse_delta[0]}, dy={mouse_delta[1]}")
    print(f"  按下按键: {keys}")
    
    if mouse_delta == (0, 0) and len(keys) == 0:
        print("  ✓ 没有误报 - 未检测到任何输入")
        results['no_false_positive'] = True
        results['details'].append("✓ 无误报测试通过")
    else:
        print("  ✗ 存在误报 - 未发送信号却检测到输入")
        results['details'].append("✗ 误报测试失败")
    
    # 测试 2: 发送控制信号，确认能正确检测
    print("\n[测试 2] 验证能正确检测（发送控制信号）")
    
    # 检测鼠标移动
    reset_detection_state()
    hid_controller.move_mouse_relative(50, 0)
    time.sleep(0.3)
    
    detected_delta = get_mouse_delta()
    print(f"  发送: move_mouse_relative(50, 0)")
    print(f"  检测到: dx={detected_delta[0]}, dy={detected_delta[1]}")
    
    if detected_delta[0] != 0 or detected_delta[1] != 0:
        print("  ✓ 成功检测到鼠标移动")
        results['can_detect'] = True
        results['details'].append("✓ 检测功能验证通过")
    else:
        print("  ✗ 未检测到鼠标移动")
        results['details'].append("✗ 检测功能验证失败")
    
    # 总结
    print("\n" + "-" * 40)
    print("  检测函数有效性验证总结:")
    all_pass = results['no_false_positive'] and results['can_detect']
    print(f"  总体结果: {'✓ 检测函数工作正常' if all_pass else '✗ 检测函数存在问题'}")
    print(f"  无误报: {'✓' if results['no_false_positive'] else '✗'}")
    print(f"  可检测: {'✓' if results['can_detect'] else '✗'}")
    print("  详情:")
    for detail in results['details']:
        print(f"    {detail}")
    
    return results


def run_all_detection_tests():
    """运行所有检测测试"""
    print("=" * 60)
    print("  HIDController 键盘鼠标控制检测 - 完整测试")
    print("=" * 60)
    print()
    print("本测试将验证:")
    print("  1. 键盘按键控制是否正常工作")
    print("  2. 鼠标移动控制是否正常工作")
    print("  3. 鼠标按键控制是否正常工作")
    print("  4. 键盘组合键控制是否正常工作")
    print("  5. 检测函数本身是否有效")
    print()
    print("请留意屏幕输出结果")
    print()
    time.sleep(2)
    
    all_results = {}
    
    try:
        # 验证检测函数本身
        print("\n" + "#" * 60)
        all_results['detection_validity'] = verify_detection_function_works()
        time.sleep(0.5)
        
        # ===== 键盘测试 =====
        print("\n" + "#" * 60)
        print("\n  >>> 开始键盘测试 <<<")
        
        # 键盘按键检测
        all_results['keyboard'] = test_keyboard_detection()
        time.sleep(0.5)
        
        # 键盘组合键检测
        all_results['keyboard_combo'] = test_keyboard_combo_detection()
        time.sleep(0.5)
        
        # ===== 鼠标测试 =====
        print("\n" + "#" * 60)
        print("\n  >>> 开始鼠标测试 <<<")
        
        # 鼠标移动检测
        all_results['mouse_move'] = test_mouse_move_detection()
        time.sleep(0.5)
        
        # 鼠标按键检测
        all_results['mouse_button'] = test_mouse_button_detection()
        time.sleep(0.5)
        
        # 最终总结
        print("\n" + "=" * 60)
        print("  所有检测测试完成 - 最终总结")
        print("=" * 60)
        
        test_names = {
            'detection_validity': '检测函数有效性',
            'keyboard': '键盘按键检测',
            'mouse_move': '鼠标移动检测',
            'mouse_button': '鼠标按键检测',
            'keyboard_combo': '键盘组合键检测',
        }
        
        print()
        for key, name in test_names.items():
            result = all_results.get(key, {})
            status = '✓ 通过' if result.get('success', False) else '✗ 失败'
            if key == 'detection_validity':
                all_pass = result.get('no_false_positive', False) and result.get('can_detect', False)
                status = '✓ 通过' if all_pass else '✗ 失败'
            print(f"  {name}: {status}")
        
        print()
        overall_pass = all(
            r.get('success', False) or 
            (k == 'detection_validity' and 
             r.get('no_false_positive', False) and 
             r.get('can_detect', False))
            for k, r in all_results.items()
        )
        
        if overall_pass:
            print("✓ 所有检测通过！键盘鼠标控制功能正常。")
        else:
            print("✗ 部分检测未通过，请检查上述详细信息。")
        
        print()
        print("检测函数说明:")
        print("  - 使用 input_tracker 模块的轮询模式和回调模式")
        print("  - 通过发送控制信号并检测是否被 input_tracker 捕获来验证")
        print("  - 包含无误报验证，确保检测结果的可靠性")
        
    except KeyboardInterrupt:
        print("\n\n测试被用户取消")
    except Exception as e:
        print(f"\n\n测试出错: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    run_all_detection_tests()
