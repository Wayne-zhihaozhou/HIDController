//DriveMouse.hpp
#pragma once
//#include "SendTypes.hpp"
//#include "IbSendBase.hpp"
//#include "IbSendMouse.hpp"
//#include "IbSendInput.hpp"
//#include "IbSendKeyboard.hpp"




// IbSendMouseCAPI.hpp
#pragma once
#include "dll_export.hpp"
#include <SendTypes.hpp>


// ----------------------------
// DLL 导入/导出宏定义
// ----------------------------
#ifdef DLL1_EXPORTS
// 编译 DLL 时定义 DLL1_EXPORTS，导出函数
#define DLLAPI extern "C" __declspec(dllexport)
#else
// 客户端使用 DLL 时，导入函数
#define DLLAPI extern "C" __declspec(dllimport)
#endif

// ----------------------------
// 调用约定宏（可选）
// ----------------------------
#ifndef WINAPI
#define WINAPI WINAPI  // Windows 默认调用约定
#endif

// ----------------------------
// 枚举示例
// ----------------------------
typedef enum {
    MOVE_ABSOLUTE = 0,
    MOVE_RELATIVE = 1
} MoveMode;

#ifdef __cplusplus
extern "C" {
#endif

    // ----------------------------
    // DLL 导出函数声明示例
    // ----------------------------

    // 鼠标移动
    DLLAPI bool WINAPI IbSendMouse_Move(int x, int y, MoveMode mode);

    // 鼠标点击
    DLLAPI bool WINAPI IbSendMouse_Click(int button);

    // 鼠标滚轮
    DLLAPI bool WINAPI IbSendMouse_Wheel(int movement);

#ifdef __cplusplus
}
#endif


/*

// DriverMouseAPI.hpp
#pragma once

#ifdef DRIVER_MOUSE_EXPORTS
#define DLLAPI extern "C" __declspec(dllexport)
#else
#define DLLAPI extern "C" __declspec(dllimport)
#endif

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



*/

