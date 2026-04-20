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

| 函数 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| `MouseMoveRelative(dx, dy)` | `dx`: int, `dy`: int | `bool` | 相对移动鼠标 |
| `MouseMoveAbsolute(x, y)` | `x`: int, `y`: int | `bool` | 绝对移动鼠标到指定位置 |
| `MouseDown(button)` | `button`: int | `bool` | 鼠标按键按下 |
| `MouseUp(button)` | `button`: int | `bool` | 鼠标按键抬起 |
| `MouseClick(button)` | `button`: int | `bool` | 鼠标单击（按下+抬起） |
| `MouseWheel(movement)` | `movement`: int | `bool` | 鼠标滚轮滚动（120 = 一格） |
| `SetMouseMoveCoefficient(coefficient)` | `coefficient`: float | `bool` | 设置鼠标移动速度系数 |
| `AutoCalibrate()` | 无 | `bool` | 自动校准鼠标速度系数 |
| `DisableMouseAcceleration()` | 无 | `bool` | 禁用 Windows 鼠标加速 |
| `EnableMouseAcceleration()` | 无 | `bool` | 启用 Windows 鼠标加速 |

**鼠标按钮常量**

| 常量 | 值 | 说明 |
|------|------|------|
| `MouseEvent.LEFT` | `0x02` | 左键 |
| `MouseEvent.RIGHT` | `0x04` | 右键 |
| `MouseEvent.MIDDLE` | `0x20` | 中键（滚轮） |

使用示例：
```python
hid_controller.MouseClick(0x02)   # 左键单击
hid_controller.MouseClick(0x04)   # 右键单击
hid_controller.MouseClick(0x20)   # 中键单击
```

### 键盘函数

| 函数 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| `KeyDown(vk)` | `vk`: int 或 str | `bool` | 按键按下 |
| `KeyUp(vk)` | `vk`: int 或 str | `bool` | 按键抬起 |
| `KeyPress(vk)` | `vk`: int 或 str | `bool` | 按键（按下+抬起） |
| `KeyCombo(keys)` | `keys`: list[int 或 str] | `bool` | 组合键（同时按下多个键，然后反向释放） |
| `KeySeq(keys)` | `keys`: list[int 或 str] | `bool` | 按键序列（依次按下并释放每个键） |
| `release_all_keys()` | 无 | 无 | 释放所有按键 |

**使用字符串键名示例**：
```python
hid_controller.KeyDown('a')           # 按下 'a' 键
hid_controller.KeyDown('lctrl')       # 按下左 Ctrl
hid_controller.KeyPress('Enter')      # 按下并抬起 Enter
hid_controller.KeyCombo(['lctrl', 'c'])  # Ctrl+C 组合键
hid_controller.KeySeq(['a', 'b', 'c'])   # 依次按下 a, b, c
```

**使用虚拟键码示例**：
```python
hid_controller.KeyDown(65)            # 按下 'A' 键 (VK_A = 65)
hid_controller.KeyDown(162)           # 按下左 Ctrl (VK_LCONTROL = 162)
hid_controller.KeyPress(13)           # Enter (VK_RETURN = 13)
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
├── pyproject.toml       # 现代化 Python 包配置
├── README.md            # 项目文档
├── LICENSE              # MIT 许可证
├── .gitignore
├── test_all_api_auto.py         # 完整 API 自动测试
├── hid_controller.*.pyd   # 预编译的 Windows x64 扩展
├── include/             # 头文件
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
    ├── keyboard_example.py
    └── advanced_example.py
```

## 构建

从源码构建：

```bash
python setup.py build_ext --inplace
```

## 测试

### 运行完整 API 测试
```bash
python test_all_api_auto.py
```

### 运行示例代码
```bash
python examples/mouse_example.py
python examples/keyboard_example.py
python examples/advanced_example.py
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
