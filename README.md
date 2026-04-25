# HIDController

通过 Logitech 虚拟驱动发送 HID 报告来控制键盘鼠标的 Python 扩展。

> **重要提示**：使用前必须安装并启动 [Logitech Gaming Software (LGS)](https://www.logitechg.com/zh-cn/software/lgs)。该扩展通过向 Logitech 虚拟驱动发送 HID 报告来实现键盘鼠标控制。

## 功能概述

本项目包含两个核心模块：

| 模块 | 功能 | 技术 |
|------|------|------|
| **主模块** | 控制键盘鼠标（输出） | Logitech HID 报告 |
| **input_tracker** | 检测键盘鼠标事件（输入） | Windows RAW INPUT API |

## 环境要求

- Python 3.8+
- Logitech Gaming Software (LGS)
- Windows 10/11

通过 pip 安装：

```bash
pip install hid-controller
```

## API 参考

### 鼠标函数

| 函数 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| `move_mouse_relative(dx, dy)` | `dx`: int, `dy`: int | `bool` | 相对移动鼠标 |
| `move_mouse_absolute(x, y)` | `x`: int, `y`: int | `bool` | 绝对移动鼠标到指定位置 |
| `mouse_down(button)` | `button`: int 或 str | `bool` | 鼠标按键按下 |
| `mouse_up(button)` | `button`: int 或 str | `bool` | 鼠标按键抬起 |
| `mouse_press(button)` | `button`: int 或 str | `bool` | 鼠标单击（按下+抬起） |
| `mouse_click(button)` | `button`: int 或 str | `bool` | 鼠标单击（按下+抬起） |
| `mouse_wheel(movement)` | `movement`: int | `bool` | 鼠标滚轮滚动（120 = 一格） |
| `set_mouse_move_coefficient(coefficient)` | `coefficient`: float | `bool` | 设置鼠标移动速度系数 |
| `auto_calibrate()` | 无 | `bool` | 自动校准鼠标速度系数 |
| `disable_mouse_acceleration()` | 无 | `bool` | 禁用 Windows 鼠标加速 |
| `enable_mouse_acceleration()` | 无 | `bool` | 启用 Windows 鼠标加速 |

**鼠标按钮常量（MouseEvent 类）**

| 常量 | 字符串 | 说明 |
|------|--------|------|
| `MouseEvent.LEFT` | `"left"` | 左键 |
| `MouseEvent.RIGHT` | `"right"` | 右键 |
| `MouseEvent.MIDDLE` | `"middle"` | 中键 |
| `MouseEvent.XBUTTON1_DOWN` | `"xbutton1"` | X 按钮 1 |
| `MouseEvent.XBUTTON2_DOWN` | `"xbutton2"` | X 按钮 2 |

**使用字符串方式（推荐）**：
```python
hid_controller.mouse_click("left")        # 左键单击
hid_controller.mouse_press("right")       # 右键单击
hid_controller.mouse_down("middle")       # 中键按下
hid_controller.mouse_up("middle")         # 中键释放
hid_controller.mouse_click("xbutton1")    # X 按钮 1 单击
hid_controller.move_mouse_relative(100, 50)   # 相对移动
hid_controller.mouse_wheel(120)               # 滚轮向上
```

**使用 MouseEvent 常量方式(有 IDE 补全)**：
```python
hid_controller.mouse_click(hid_controller.MouseEvent.LEFT)    # 左键单击
hid_controller.mouse_click(hid_controller.MouseEvent.RIGHT)   # 右键单击
```

**使用数字常量方式（传统方式，仍然支持）**：
```python
hid_controller.mouse_click(0x02)    # 左键单击
hid_controller.move_mouse_relative(100, 50)   # 相对移动
hid_controller.mouse_wheel(120)               # 滚轮向上
```

### 键盘函数

| 函数 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| `key_down(vk)` | `vk`: int 或 str | `bool` | 按键按下 |
| `key_up(vk)` | `vk`: int 或 str | `bool` | 按键抬起 |
| `key_press(vk)` | `vk`: int 或 str | `bool` | 按键（按下+抬起） |
| `key_combo(keys)` | `keys`: list[int 或 str] | `bool` | 组合键（同时按下多个键，然后反向释放） |
| `key_seq(keys)` | `keys`: list[int 或 str] | `bool` | 按键序列（依次按下并释放每个键） |
| `release_all_keys()` | 无 | 无 | 释放所有按键 |

**键盘键码常量（KeyEvent 类）**

| 常量组 | 包含的常量 |
|--------|-----------|
| 修饰键 | `LCONTROL`, `RCONTROL`, `LSHIFT`, `RSHIFT`, `LMENU`, `RMENU`, `LWIN`, `RWIN`, `CONTROL`, `SHIFT`, `MENU` |
| 功能键 | `F1` - `F12` |
| 控制键 | `RETURN`, `ESCAPE`, `TAB`, `BACK`, `DELETE`, `INSERT`, `HOME`, `END`, `PRIOR`, `NEXT`, `SPACE` |
| 方向键 | `UP`, `DOWN`, `LEFT`, `RIGHT` |
| 状态键 | `CAPITAL`, `NUMLOCK`, `SCROLL` |
| 字母 | `A` - `Z` |
| 数字 | `KEY_0` - `KEY_9` |

**使用字符串方式（推荐，简单易记，全部小写）**：
```python
hid_controller.key_down('a')              # 按下 'a' 键
hid_controller.key_down('lctrl')          # 按下左 Ctrl
hid_controller.key_press('enter')         # 按下并抬起 Enter
hid_controller.key_combo(['lctrl', 'c'])  # Ctrl+C 组合键
hid_controller.key_seq(['a', 'b', 'c'])   # 依次按下 a, b, c
```

**使用 KeyEvent 常量方式（有 IDE 补全）**：
```python
hid_controller.key_press(hid_controller.KeyEvent.A)           # 按下 'A' 键
hid_controller.key_down(hid_controller.KeyEvent.LCONTROL)     # 按下左 Ctrl
hid_controller.key_press(hid_controller.KeyEvent.RETURN)      # Enter
hid_controller.key_combo([hid_controller.KeyEvent.LCONTROL, hid_controller.KeyEvent.C])  # Ctrl+C
hid_controller.key_seq([hid_controller.KeyEvent.KEY_1, hid_controller.KeyEvent.KEY_2])  # 按下 1, 2
```

**使用虚拟键码方式（传统方式，仍然支持）**：
```python
hid_controller.key_down(65)            # 按下 'A' 键 (VK_A = 65)
hid_controller.key_down(162)           # 按下左 Ctrl (VK_LCONTROL = 162)
hid_controller.key_press(13)           # Enter (VK_RETURN = 13)
```

**支持的字符串键名**：

| 键名 | 说明 | 键名 | 说明 |
|------|------|------|------|
| `'a'` - `'z'` | 字母 A-Z | `'0'` - `'9'` | 数字 0-9 |
| `'space'` | 空格 | `'enter'` | Enter |
| `'shift'` | 左/右 Shift | `'ctrl'` | 左/右 Ctrl |
| `'alt'` | 左/右 Alt | `'tab'` | Tab |
| `'escape'` | Esc | `'back'` | Backspace |
| `'delete'` | Delete | `'insert'` | Insert |
| `'home'` | Home | `'end'` | End |
| `'pageup'` | Page Up | `'pagedown'` | Page Down |
| `'up'`, `'down'`, `'left'`, `'right'` | 方向键 | `'f1'` - `'f12'` | 功能键 |
| `'lshift'`, `'rshift'` | 左/右 Shift | `'lctrl'`, `'rctrl'` | 左/右 Ctrl |
| `'lalt'`, `'ralt'` | 左/右 Alt | `'win'` | Win 键 |

## input_tracker API 参考（输入检测）

### 函数列表

| 函数 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| `start(mouse_callback, key_callback, mouse_button_callback)` | 回调函数 | `None` | 开始跟踪键盘鼠标事件 |
| `stop()` | 无 | `None` | 停止跟踪 |
| `is_tracking()` | 无 | `bool` | 检查是否正在跟踪 |
| `get_mouse_delta()` | 无 | `(dx, dy)` | 获取鼠标增量（轮询模式） |
| `get_pressed_keys()` | 无 | `list` | 获取当前按下的键（轮询模式） |

### 回调函数签名

```python
def mouse_callback(device_handle: int, dx: int, dy: int) -> None
def key_callback(device_handle: int, vkey: int, is_down: bool) -> None
def mouse_button_callback(device_handle: int, button: int, is_down: bool) -> None
```

**参数说明**：
- `device_handle`: 设备句柄（uintptr_t）
- `dx`, `dy`: 鼠标位移量
- `vkey`: 虚拟键码 (VK_*)
- `is_down`: True=按下, False=抬起
- `button`: 1=左键, 2=右键, 3=中键

### 使用示例

**回调模式（事件驱动，推荐）**：
```python
import hid_controller.input_tracker as tracker

def on_mouse(device, dx, dy):
    print(f"Mouse: dx={dx}, dy={dy}")

def on_key(device, vkey, is_down):
    state = "DOWN" if is_down else "UP"
    print(f"Key {state}: vkey={vkey}")

def on_mouse_button(device, button, is_down):
    btn = {1: "Left", 2: "Right", 3: "Middle"}.get(button, str(button))
    state = "DOWN" if is_down else "UP"
    print(f"Mouse {btn} {state}")

# 启动跟踪
tracker.start(on_mouse, on_key, on_mouse_button)

# ... 运行你的代码 ...

# 停止跟踪
tracker.stop()
```

**轮询模式（主动查询）**：
```python
import hid_controller.input_tracker as tracker

# 启动跟踪（不传回调）
tracker.start()

# 运行你的代码...
dx, dy = tracker.get_mouse_delta()
keys = tracker.get_pressed_keys()

# 停止跟踪
tracker.stop()
```

### 通过主模块访问

```python
import hid_controller

# 方式 1: 直接导入子模块
import hid_controller.input_tracker as tracker

# 方式 2: 通过主模块访问
tracker = hid_controller._get_input_tracker()
```

## 项目结构

```
HIDController/
├── binding.cpp          # pybind11 绑定（主模块：输出控制）
├── setup.py             # Python 扩展构建配置
├── pyproject.toml       # 现代化 Python 包配置（含 cibuildwheel 配置）
├── README.md            # 项目文档
├── LICENSE              # MIT 许可证
├── .gitignore
├── test_all_api_auto.py         # 完整 API 自动测试
├── hid_controller/      # Python 包
│   ├── __init__.py      # 包入口
│   └── input_tracker.py # 输入检测模块包装
├── include/             # C++ 头文件
│   ├── HIDController.hpp
│   ├── KeyboardMap.hpp
│   ├── Logitech.hpp
│   ├── LogitechDriver.hpp
│   └── pch.h
├── src/                 # C++ 源文件
│   ├── IbSendMouse.cpp
│   ├── IbSendKeyboard.cpp
│   ├── Logitech.cpp
│   ├── LogitechDriver.cpp
│   ├── pch.cpp
│   └── InputTracker.cpp # 输入检测模块（RAW INPUT）
└── examples/            # 示例代码
    ├── mouse_example.py
    ├── keyboard_example.py
    └── input_tracker_example.py  # 输入检测示例
```

## 构建 wheel 包

```bash
pip install pybind11 setuptools wheel
python setup.py bdist_wheel
```

wheel 包生成在 `dist/` 目录下。

## 测试

### API 测试
```bash
python test_detection.py
```

### 运行示例代码
```bash
python examples/mouse_example.py
python examples/keyboard_example.py
python examples/input_tracker_example.py
```



## 免责声明

> ⚠️ **使用本软件即表示您同意以下条款：**
>
> 本软件按"原样"提供，不提供任何形式的明示或暗示保证，包括但不限于对适销性、特定用途适用性的保证。作者不对因使用本软件而产生的任何直接、间接、偶然、实质性或随之产生的损害负责。
>
> **作者不承担法律责任：**
> - 因使用本软件导致的任何数据丢失、系统损坏或数据泄露
> - 因使用本软件导致的任何直接或间接损失
> - 因使用本软件导致的第三方索赔或法律责任
>
> **使用者责任：**
> - 使用者需自行评估和承担使用本软件的风险
> - 请确保使用本软件符合当地法律法规
> - 作者不对软件的可能滥用行为负责

## 许可证

本项目采用 [MIT License](LICENSE) - 一种宽松的开源许可证，允许任何人自由使用、修改和分发代码。

**MIT License 是什么意思？**
- ✅ 你可以自由使用、修改、分发本代码
- ✅ 可用于商业项目
- ✅ 可以修改后闭源
- 📝 只需保留原始许可证和版权声明
