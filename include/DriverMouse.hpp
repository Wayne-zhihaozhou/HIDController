//DriveMouse.hpp
#pragma once
#include <SendTypes.hpp>



//--------------------定义DLL导出宏----------
#ifdef DLL1_EXPORTS
#define DLLAPI extern "C" __declspec(dllexport)//C语言链接方式;函数导出到DLL中
#else
#define DLLAPI extern "C" __declspec(dllimport)//C语言链接方式;函数导入到DLL中
#endif


//--------------------枚举--------------------

#ifdef __cplusplus
extern "C" {
#endif

// -------------------- 初始化/销毁 --------------------

// 初始化模块（加载驱动等）
// 返回 true 表示成功
DLLAPI bool InitializeDriverMouse();

// 卸载模块
DLLAPI void ShutdownDriverMouse();

// -------------------- 鼠标控制 --------------------

// 相对移动鼠标 dx/dy 单位为像素
DLLAPI void MoveMouseRelative(int dx, int dy);

// 绝对移动鼠标到屏幕坐标 x/y
DLLAPI void MoveMouseAbsolute(int x, int y);

// 设置鼠标相对移动加成系数
DLLAPI void SetMouseSensitivity(float factor);

// -------------------- 键盘控制 --------------------

// 按下某个虚拟键码
DLLAPI void KeyDown(int vKey);

// 松开某个虚拟键码
DLLAPI void KeyUp(int vKey);

// 发送组合按键（例如 Ctrl + C）
// vKeys 是虚拟键码数组，count 为数组长度
DLLAPI void SendKeyCombination(const int* vKeys, int count);

#ifdef __cplusplus
}
#endif





