# HIDController DLL

独立的 C++ DLL 库，提供鼠标和键盘控制功能。

## 构建方式

### 方式一：Visual Studio

1. 打开 `projects/dll/HIDController.sln` 文件
2. 选择 `Release|x64` 配置
3. 生成解决方案

输出文件：
- `projects/dll/build/Release/HIDController.dll` - DLL 文件
- `projects/dll/build/Release/HIDController.lib` - 导入库文件

### 方式二：CMake

```bash
cd projects/dll
mkdir build_dll
cd build_dll
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

输出文件：
- `projects/dll/build_dll/bin/HIDController.dll` - DLL 文件
- `projects/dll/build_dll/lib/HIDController.lib` - 导入库文件

## 使用方法

### 链接方式

```cpp
#include "hid_controller.h"
#pragma comment(lib, "HIDController.lib")

int main() {
    // 鼠标点击
    mouse_click(0x01);  // 左键点击

    // 键盘按下
    key_down(0x41);     // A 键按下
    key_up(0x41);       // A 键抬起

    return 0;
}
```

### 动态加载方式

```cpp
#include <windows.h>

typedef bool (__stdcall *mouse_move_relative_func)(int32_t, int32_t);

int main() {
    HMODULE hDll = LoadLibrary(L"HIDController.dll");
    if (hDll) {
        mouse_move_relative_func pFunc = (mouse_move_relative_func)GetProcAddress(hDll, "mouse_move_relative");
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
| `mouse_move_relative(dx, dy)` | 相对鼠标移动 |
| `mouse_move_absolute(x, y)` | 绝对鼠标移动 |
| `mouse_down(button)` | 鼠标按键按下 |
| `mouse_up(button)` | 鼠标按键抬起 |
| `mouse_click(button)` | 鼠标点击 |
| `mouse_wheel(movement)` | 鼠标滚轮 |
| `set_mouse_move_coefficient(coeff)` | 设置移动系数 |
| `auto_calibrate()` | 自动校准 |
| `disable_mouse_acceleration()` | 禁用鼠标加速 |
| `enable_mouse_acceleration()` | 启用鼠标加速 |

### 键盘控制

| 函数 | 描述 |
|------|------|
| `key_down(vk)` | 键盘按键按下 |
| `key_up(vk)` | 键盘按键抬起 |
| `key_press(vk)` | 键盘按键 |
| `key_combo(keys)` | 组合键（同时按下） |
| `key_seq(keys)` | 按键序列（依次按下） |
| `release_all_keys()` | 释放所有按键 |
