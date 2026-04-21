# HIDController

通过 Logitech 虚拟驱动发送 HID 报告来控制键盘鼠标的 Python 扩展。

> **重要提示**：使用前必须安装并启动 [Logitech Gaming Software (LGS)](https://www.logitechg.com/zh-cn/software/lgs)。该扩展通过向 Logitech 虚拟驱动发送 HID 报告来实现键盘鼠标控制，绕过游戏/系统的检测。

## 环境要求

- Python 3.8+
- Logitech Gaming Software (LGS)

通过 pip 安装：

```bash
pip install hid-controller
```

## 快速开始

```python
import hid_controller

# ==================== 鼠标控制 ====================

# 相对移动
hid_controller.move_mouse_relative(100, 50)

# 绝对移动
hid_controller.move_mouse_absolute(500, 300)

# 鼠标按键
hid_controller.mouse_down(0x02)       # 左键按下
hid_controller.mouse_up(0x02)         # 左键抬起
hid_controller.mouse_click(0x02)      # 左键单击

# 鼠标滚轮
hid_controller.mouse_wheel(120)       # 向上滚动一格
hid_controller.mouse_wheel(-120)      # 向下滚动一格

# 设置鼠标速度系数
hid_controller.set_mouse_move_coefficient(1.5)

# 自动校准鼠标速度
hid_controller.auto_calibrate()

# 禁用/启用鼠标加速
hid_controller.disable_mouse_acceleration()
hid_controller.enable_mouse_acceleration()

# ==================== 键盘控制 ====================

# 按键（支持整数虚拟键码或字符串键名）
hid_controller.key_down('a')          # 按下 'a' 键
hid_controller.key_up('a')            # 抬起 'a' 键
hid_controller.key_press('Enter')     # 按下并抬起 Enter 键

# 组合键（同时按下多个键，然后反向释放）
hid_controller.key_combo(['lctrl', 'c'])  # Ctrl+C

# 按键序列（依次按下并释放每个键）
hid_controller.key_seq(['a', 'b', 'c'])   # 依次按下 a, b, c

# 释放所有按键
hid_controller.release_all_keys()
```

## API 参考

### 鼠标函数

| 函数 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| `move_mouse_relative(dx, dy)` | `dx`: int, `dy`: int | `bool` | 相对移动鼠标 |
| `move_mouse_absolute(x, y)` | `x`: int, `y`: int | `bool` | 绝对移动鼠标到指定位置 |
| `mouse_down(button)` | `button`: int | `bool` | 鼠标按键按下 |
| `mouse_up(button)` | `button`: int | `bool` | 鼠标按键抬起 |
| `mouse_click(button)` | `button`: int | `bool` | 鼠标单击（按下+抬起） |
| `mouse_wheel(movement)` | `movement`: int | `bool` | 鼠标滚轮滚动（120 = 一格） |
| `set_mouse_move_coefficient(coefficient)` | `coefficient`: float | `bool` | 设置鼠标移动速度系数 |
| `auto_calibrate()` | 无 | `bool` | 自动校准鼠标速度系数 |
| `disable_mouse_acceleration()` | 无 | `bool` | 禁用 Windows 鼠标加速 |
| `enable_mouse_acceleration()` | 无 | `bool` | 启用 Windows 鼠标加速 |

**鼠标按钮常量**

| 常量 | 值 | 说明 |
|------|------|------|
| `MouseEvent.LEFT` | `0x02` | 左键 |
| `MouseEvent.RIGHT` | `0x04` | 右键 |
| `MouseEvent.MIDDLE` | `0x20` | 中键（滚轮） |

使用示例：
```python
hid_controller.mouse_click(0x02)   # 左键单击
hid_controller.mouse_click(0x04)   # 右键单击
hid_controller.mouse_click(0x20)   # 中键单击
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

**使用字符串键名示例**：
```python
hid_controller.key_down('a')           # 按下 'a' 键
hid_controller.key_down('lctrl')       # 按下左 Ctrl
hid_controller.key_press('Enter')      # 按下并抬起 Enter
hid_controller.key_combo(['lctrl', 'c'])  # Ctrl+C 组合键
hid_controller.key_seq(['a', 'b', 'c'])   # 依次按下 a, b, c
```

**使用虚拟键码示例**：
```python
hid_controller.key_down(65)            # 按下 'A' 键 (VK_A = 65)
hid_controller.key_down(162)           # 按下左 Ctrl (VK_LCONTROL = 162)
hid_controller.key_press(13)           # Enter (VK_RETURN = 13)
```

### 支持的键盘键名

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

## 项目结构

```
HIDController/
├── binding.cpp          # pybind11 绑定
├── setup.py             # Python 扩展构建配置
├── pyproject.toml       # 现代化 Python 包配置（含 cibuildwheel 配置）
├── README.md            # 项目文档
├── LICENSE              # MIT 许可证
├── .gitignore
├── test_all_api_auto.py         # 完整 API 自动测试
├── hid_controller/      # Python 包
│   └── __init__.py      # 包入口
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
│   └── pch.cpp
└── examples/            # 示例代码
    ├── mouse_example.py
    └── keyboard_example.py
```

## 构建

```bash
pip install pybind11 setuptools wheel
python setup.py bdist_wheel
```

wheel 包生成在 `dist/` 目录下。

## 测试

### 运行完整 API 测试
```bash
python test_all_api_auto.py
```

### 运行示例代码
```bash
python examples/mouse_example.py
python examples/keyboard_example.py
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
