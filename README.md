# HIDController

通过 Logitech 虚拟驱动发送 HID 报告来控制键盘鼠标的 Python 扩展。

> **重要提示**：使用前必须安装并启动 [Logitech Gaming Software (LGS)](https://www.logitechg.com/zh-cn/software/lgs)。该扩展通过向 Logitech 虚拟驱动发送 HID 报告来实现键盘鼠标控制，绕过游戏/系统的检测。

## 安装

### 环境要求

- Python 3.12+
- C++ 编译器（Visual Studio Build Tools 或 Visual Studio）
- Logitech Gaming Software (LGS)

### 安装步骤

```bash
# 安装 pybind11 依赖
pip install pybind11

# 从源码构建安装
pip install .
```

或者使用开发模式（修改代码后无需重新安装）：

```bash
pip install -e .
```

## 快速开始

```python
import hid_controller

# ==================== 鼠标控制 ====================

# 相对移动
hid_controller.MouseMoveRelative(100, 50)

# 绝对移动
hid_controller.MouseMoveAbsolute(500, 300)

# 鼠标按键
hid_controller.MouseDown(0x02)       # 左键按下
hid_controller.MouseUp(0x02)         # 左键抬起
hid_controller.MouseClick(0x02)      # 左键单击

# 鼠标滚轮
hid_controller.MouseWheel(120)       # 向上滚动一格
hid_controller.MouseWheel(-120)      # 向下滚动一格

# 设置鼠标速度系数
hid_controller.SetMouseMoveCoefficient(1.5)

# 自动校准鼠标速度
hid_controller.AutoCalibrate()

# 禁用/启用鼠标加速
hid_controller.DisableMouseAcceleration()
hid_controller.EnableMouseAcceleration()

# ==================== 键盘控制 ====================

# 按键（支持整数虚拟键码或字符串键名）
hid_controller.KeyDown('a')          # 按下 'a' 键
hid_controller.KeyUp('a')            # 抬起 'a' 键
hid_controller.KeyPress('Enter')     # 按下并抬起 Enter 键

# 组合键（同时按下多个键，然后反向释放）
hid_controller.KeyCombo(['lctrl', 'c'])  # Ctrl+C

# 按键序列（依次按下并释放每个键）
hid_controller.KeySeq(['a', 'b', 'c'])   # 依次按下 a, b, c

# 释放所有按键
hid_controller.release_all_keys()
```

## API 参考

### 鼠标函数

| 函数 | 参数 | 说明 |
|------|------|------|
| `MouseMoveRelative(dx, dy)` | `dx`, `dy`: int | 相对移动鼠标 |
| `MouseMoveAbsolute(x, y)` | `x`, `y`: int | 绝对移动鼠标到指定位置 |
| `MouseDown(button)` | `button`: int | 鼠标按键按下 |
| `MouseUp(button)` | `button`: int | 鼠标按键抬起 |
| `MouseClick(button)` | `button`: int | 鼠标单击（按下+抬起） |
| `MouseWheel(movement)` | `movement`: int | 鼠标滚轮滚动（120 = 一格） |
| `SetMouseMoveCoefficient(coeff)` | `coeff`: float | 设置鼠标移动速度系数 |
| `AutoCalibrate()` | 无 | 自动校准鼠标速度系数 |
| `DisableMouseAcceleration()` | 无 | 禁用 Windows 鼠标加速 |
| `EnableMouseAcceleration()` | 无 | 启用 Windows 鼠标加速 |

### 键盘函数

| 函数 | 参数 | 说明 |
|------|------|------|
| `KeyDown(vk)` | `vk`: int 或 str | 按键按下 |
| `KeyUp(vk)` | `vk`: int 或 str | 按键抬起 |
| `KeyPress(vk)` | `vk`: int 或 str | 按键（按下+抬起） |
| `KeyCombo(keys)` | `keys`: list[int 或 str] | 组合键 |
| `KeySeq(keys)` | `keys`: list[int 或 str] | 按键序列 |
| `release_all_keys()` | 无 | 释放所有按键 |

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
├── pyproject.toml       # 现代化 Python 包配置
├── README.md            # 项目文档
├── .gitignore
├── include/             # 头文件
│   ├── HIDController.hpp
│   ├── KeyboardMap.hpp
│   ├── Logitech.hpp
│   ├── LogitechDriver.hpp
│   └── pch.h
└── src/                 # C++ 源文件
    ├── IbSendMouse.cpp
    ├── IbSendKeyboard.cpp
    ├── Logitech.cpp
    ├── LogitechDriver.cpp
    └── pch.cpp
```

## 构建

从源码构建：

```bash
python setup.py build
```

构建并打包：

```bash
python -m build
```

## 许可证

MIT License
