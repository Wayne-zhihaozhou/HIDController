# DriverMouse DLL

## 项目简介
`DriverMouse.dll` 是一个 C++ 动态链接库，提供鼠标与键盘的底层控制能力，支持相对移动、点击、滚轮操作以及按键模拟等功能。  

通过向 **Logitech Gaming Software (LGS)** 创建的虚拟驱动发送 **HID 报告** 来模拟输入事件，因此使用前必须安装并启动 LGS 以提供底层环境支持。



---

## 环境依赖

- **Logitech Gaming Software (LGS)**  
  - 下载地址：[https://filehippo.com/download_logitech-gaming-software/9.02.65/](https://filehippo.com/download_logitech-gaming-software/9.02.65/)  
  - **重要**：系统启动后必须手动运行一次 LGS，否则 DLL 无法正常操作鼠标。

---

## 使用示例

### 使用时包含唯一头文件：

```cpp
//需要提前添加 dll 和 lib 到项目中
#include "HIDController.hpp"

// -------------------- 鼠标控制 -------------------

// 相对当前位置向右移动 500 像素
MouseMoveRelative(500, 0);

// 移动鼠标到屏幕中心 (分辨率 1920x1080)
MouseMoveAbsolute(960, 540);

// 按下鼠标左键
MouseDown(MOUSEEVENTF_LEFTDOWN);

// 松开鼠标左键
MouseUp(MOUSEEVENTF_LEFTUP);

// 鼠标左键点击（按下 + 松开）
MouseClick(MOUSEEVENTF_LEFTDOWN);

// 鼠标向上滚动一个标准单位
MouseWheel(120);

// 设置鼠标移动灵敏度系数为 1.5
SetMouseMoveCoefficient(1.5f);

// 自动校准鼠标
AutoCalibrate();

// 禁用鼠标加速度
DisableMouseAcceleration();

// 启用鼠标加速度
EnableMouseAcceleration();


// -------------------- 键盘控制 -------------------

// 按下键盘 "A" 键
KeyDown('A');

// 松开键盘 "A" 键
KeyUp('A');

// 点击键盘 "Enter" 键
KeyPress(VK_RETURN);

// 同时按下 Shift + Alt + T 组合键
KeyCombo({ VK_LSHIFT, VK_MENU, 'T' });

// 连续按键
KeySeq({ 'Z', 'U', 'H', 'E', 'A', 'N', 'J', 'I', 'A', 'N' });

```

## 注意事项

1. 使用 **鼠标相对移动** 前，需要校准鼠标灵敏度系数：
   - 手动校准：`SetMouseMoveCoefficient()`
   - 自动校准：`AutoCalibrate()`

2. 使用 **鼠标相对移动** 前，需要禁用 Windows 系统鼠标加速度，否则移动不准确：
   - 禁用加速度：`DisableMouseAcceleration()`
   - 恢复加速度：`EnableMouseAcceleration()`

3. **鼠标绝对移动** 是通过相对移动实现，可能不准确。