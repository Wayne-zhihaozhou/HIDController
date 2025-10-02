// pch.h
//告诉编译器有哪些头是几乎每个文件都要用的。

/*

// ✅ 常用宏
#define WIN32_LEAN_AND_MEAN

// ✅ 系统头文件
#include <windows.h>
#include <thread>
#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

// ✅ 常用 STL
#include <functional>
#include <optional>

// ✅ 项目基础公共头
#include "framework.h"    // 项目级别声明
#include "base.hpp"       // 稳定底层工具类（如果几乎不变）

*/

#pragma once
#define WIN32_LEAN_AND_MEAN


// 系统和标准库头文件
#include <windows.h>
#include <mutex>
#include <memory>
#include <string>
#include <cstdint>
#include <functional>
#include <winternl.h>
#include <bit>
#include <cassert>
#include <string_view>

// 第三方库头文件
#include "../detours/detours.h"


// 项目自定义头文件
#include "base.hpp"
#include "SendTypes.hpp"
#include "SendInput.hpp"
//#include "IbSendBase.hpp"
//#include "IbSendInput.hpp"
//#include "IbSendKeyboard.hpp"
//#include "IbSendMouse.hpp"
//#include "SendInputHook.hpp"
//#include "KeyboardMap.hpp"
//#include "Logitech.hpp"
//#include "LogitechDriver.hpp"
//#include "framework.h"


