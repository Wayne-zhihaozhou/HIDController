"""
HIDController Input Tracker 示例

演示如何使用 input_tracker 模块检测键盘鼠标事件。

两种使用模式：
1. 回调模式（事件驱动）- 推荐，响应更快
2. 轮询模式（主动查询）- 适合简单场景
"""

import sys
import time


def example_callback_mode():
    """
    示例 1: 回调模式（事件驱动）
    
    这是推荐的使用方式，事件发生时会自动调用回调函数。
    """
    print("=" * 60)
    print("示例 1: 回调模式（事件驱动）")
    print("=" * 60)
    print("移动鼠标、按下按键或点击鼠标按钮来查看事件...")
    print("按 Ctrl+C 退出\n")
    
    try:
        # 导入 input_tracker 模块
        from hid_controller import input_tracker
        
        # 定义回调函数
        def on_mouse_move(device_handle, dx, dy):
            """鼠标移动回调"""
            print(f"[鼠标移动] 设备={device_handle}, 位移=({dx}, {dy})")
        
        def on_key_event(device_handle, vkey, is_down):
            """键盘回调"""
            key_state = "按下" if is_down else "抬起"
            print(f"[键盘事件] 设备={device_handle}, 虚拟键码={vkey}, {key_state}")
        
        def on_mouse_button(device_handle, button, is_down):
            """鼠标按键回调"""
            button_names = {1: "左键", 2: "右键", 3: "中键"}
            button_name = button_names.get(button, f"按钮{button}")
            button_state = "按下" if is_down else "抬起"
            print(f"[鼠标按键] 设备={device_handle}, {button_name} {button_state}")
        
        # 启动跟踪（传入回调函数）
        input_tracker.start(
            mouse_callback=on_mouse_move,
            key_callback=on_key_event,
            mouse_button_callback=on_mouse_button
        )
        
        print("跟踪已启动...")
        
        # 运行 30 秒
        for i in range(30):
            time.sleep(1)
            print(f"运行中... {i+1}/30 秒")
        
        # 停止跟踪
        input_tracker.stop()
        print("\n跟踪已停止。")
        
    except KeyboardInterrupt:
        print("\n用户中断，停止跟踪...")
        input_tracker.stop()
    except ImportError as e:
        print(f"\n错误: {e}")
        print("请运行: python setup.py build_ext --inplace")


def example_polling_mode():
    """
    示例 2: 轮询模式（主动查询）
    
    适合不需要实时回调的场景，主动查询当前状态。
    """
    print("\n" + "=" * 60)
    print("示例 2: 轮询模式（主动查询）")
    print("=" * 60)
    print("移动鼠标、按下按键，每 2 秒查询一次状态...")
    print("按 Ctrl+C 退出\n")
    
    try:
        from hid_controller import input_tracker
        
        # 启动跟踪（不传回调函数）
        input_tracker.start()
        print("跟踪已启动...")
        
        # 运行 20 秒
        for i in range(10):
            time.sleep(2)
            
            # 查询鼠标位移
            dx, dy = input_tracker.get_mouse_delta()
            print(f"[{i+1}/10] 鼠标位移: ({dx}, {dy})")
            
            # 查询当前按下的键
            pressed_keys = input_tracker.get_pressed_keys()
            if pressed_keys:
                print(f"       当前按下的键: {list(pressed_keys)}")
        
        # 停止跟踪
        input_tracker.stop()
        print("\n跟踪已停止。")
        
    except KeyboardInterrupt:
        print("\n用户中断，停止跟踪...")
        input_tracker.stop()
    except ImportError as e:
        print(f"\n错误: {e}")
        print("请运行: python setup.py build_ext --inplace")


def example_simple_mode():
    """
    示例 3: 最简单的使用方式
    
    仅检测鼠标移动。
    """
    print("\n" + "=" * 60)
    print("示例 3: 简单模式")
    print("=" * 60)
    print("移动鼠标查看位移数据...")
    print("按 Ctrl+C 退出\n")
    
    try:
        from hid_controller import input_tracker
        
        # 简化的回调函数
        def on_mouse(device, dx, dy):
            if dx != 0 or dy != 0:
                print(f"鼠标移动: ({dx}, {dy})")
        
        # 只传鼠标回调
        input_tracker.start(mouse_callback=on_mouse)
        print("跟踪已启动...")
        
        time.sleep(15)
        
        input_tracker.stop()
        print("\n跟踪已停止。")
        
    except KeyboardInterrupt:
        print("\n用户中断，停止跟踪...")
        try:
            input_tracker.stop()
        except:
            pass
    except ImportError as e:
        print(f"\n错误: {e}")
        print("请运行: python setup.py build_ext --inplace")


if __name__ == "__main__":
    print("HIDController Input Tracker 示例")
    print("=" * 60)
    print("请选择示例模式:")
    print("1. 回调模式（事件驱动，推荐）")
    print("2. 轮询模式（主动查询）")
    print("3. 简单模式（仅鼠标）")
    print()
    
    choice = input("请输入选择 (1/2/3)，直接回车默认为 1: ").strip()
    
    if choice == "1":
        example_callback_mode()
    elif choice == "2":
        example_polling_mode()
    elif choice == "3":
        example_simple_mode()
    else:
        example_callback_mode()
    
    print("\n所有示例已完成。")
