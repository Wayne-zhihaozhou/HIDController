// pch.h
#pragma once
#define WIN32_LEAN_AND_MEAN

#ifdef DLL1_EXPORTS
#define DLLAPI  extern "C" __declspec(dllexport)
#else
#define DLLAPI  extern "C" __declspec(dllimport)
#endif

// 系统和标准库头文件
#include <windows.h>
#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <winternl.h>
#include <bit>
#include <cassert>
#include <string_view>
#include <algorithm>


