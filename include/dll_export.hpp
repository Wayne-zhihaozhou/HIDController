#pragma once


#ifdef DLL1_EXPORTS
#define DLLAPI extern "C" __declspec(dllexport)//导出函数到 DLL
#else
#define DLLAPI extern "C" __declspec(dllimport)//从 DLL 导入函数
#endif


