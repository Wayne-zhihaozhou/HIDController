// pch.h

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


// 项目自定义头文件
#include "base.hpp"
#include "SendTypes.hpp"
#include "SendInput.hpp"


