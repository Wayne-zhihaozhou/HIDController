# HIDController DLL

独立的 C++ DLL 库，提供鼠标和键盘控制功能。

## 构建方式

### 方式一：Visual Studio

1. 打开 `HIDController.sln` 文件
2. 选择 `Release|x64` 配置
3. 生成解决方案

输出文件：
- `build\Release\HIDController.dll` - DLL 文件
- `build\Release\HIDController.lib` - 导入库文件

### 方式二：CMake

```bash
mkdir build_dll
cd build_dll
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

输出文件：
- `build_dll\bin\HIDController.dll` - DLL 文件
- `build_dll\lib\HIDController.lib` - 导入库文件

## 使用方法

### 链接方式

```cpp
#include "HIDController.hpp"
#pragma comment(lib, "HIDController.lib")

int main() {
    // 鼠标点击
    MouseClick(0x01);  // 左键点击
    
    // 键盘按下
    KeyDown(0x41);     // A 键按下
    KeyUp(0x41);       // A 键抬起
    
    return 0;
}
```

### 动态加载方式

```cpp
#include <windows.h>

typedef bool (__stdcall *MouseMoveRelativeFunc)(int32_t, int32_t);

int main() {
    HMODULE hDll = LoadLibrary(L"HIDController.dll");
    if (hDll) {
        MouseMoveRelativeFunc pFunc = (MouseMoveRelativeFunc)GetProcAddress(hDll, "MouseMoveRelative");
        if (pFunc) {
            pFunc(100, 0);
        }
        FreeLibrary(hDll);
    }
    return 0;
}
```

## 可用函数

### 鼠标控制

| 函数 | 描述 |
|------|------|
| `MouseMoveRelative(dx, dy)` | 相对鼠标移动 |
| `MouseMoveAbsolute(x, y)` | 绝对鼠标移动 |
| `MouseDown(button)` | 鼠标按键按下 |
| `MouseUp(button)` | 鼠标按键抬起 |
| `MouseClick(button)` | 鼠标点击 |
| `MouseWheel(movement)` | 鼠标滚轮 |
| `SetMouseMoveCoefficient(coeff)` | 设置移动系数 |
| `AutoCalibrate()` | 自动校准 |
| `DisableMouseAcceleration()` | 禁用鼠标加速 |
| `EnableMouseAcceleration()` | 启用鼠标加速 |

### 键盘控制

| 函数 | 描述 |
|------|------|
| `KeyDown(vk)` | 键盘按键按下 |
| `KeyUp(vk)` | 键盘按键抬起 |
| `KeyPress(vk)` | 键盘按键 |
| `KeyCombo(keys)` | 组合键（同时按下） |
| `KeySeq(keys)` | 按键序列（依次按下） |
| `release_all_keys()` | 释放所有按键 |

## 导出函数列表

```
MouseMoveRelative         @1
MouseMoveAbsolute         @2
MouseDown                 @3
MouseUp                   @4
MouseClick                @5
MouseWheel                @6
SetMouseMoveCoefficient   @7
AutoCalibrate             @8
DisableMouseAcceleration  @9
EnableMouseAcceleration   @10
KeyDown                   @11
KeyUp                     @12
KeyPress                  @13
KeyCombo                  @14
KeySeq                    @15
release_all_keys          @16
