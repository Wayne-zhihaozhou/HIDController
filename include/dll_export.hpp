//dll_export.hpp
#pragma once

#ifdef DLL1_EXPORTS
#define DLLAPI extern "C" __declspec(dllexport)//C语言链接方式;函数导出到DLL中
#else
#define DLLAPI extern "C" __declspec(dllimport)//C语言链接方式;函数导入到DLL中
#endif

