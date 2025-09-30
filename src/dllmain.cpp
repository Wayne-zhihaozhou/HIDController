#include "DriverMouse.hpp"

// DLL 入口函数：当 DLL 被加载/卸载或线程创建/结束时会被系统调用
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // 📌 当 DLL 被某个进程加载时调用（通常在 LoadLibrary 或首次使用时触发）
        // 这里可以进行初始化操作，如分配资源、创建对象等
        break;

    case DLL_THREAD_ATTACH:
        // 📌 当进程中的新线程创建并调用到此 DLL 时调用
        // 通常不需要处理，除非需要为每个线程分配特定资源
        break;

    case DLL_THREAD_DETACH:
        // 📌 当线程退出时调用（前提是该线程曾使用过该 DLL）
        // 可用于清理该线程专属的资源
        break;

    case DLL_PROCESS_DETACH:
        // 📌 当 DLL 被卸载时调用（进程退出或 FreeLibrary 时触发）
        // 通常在这里做资源释放、对象销毁、清理操作
        IbSendDestroy();  // 这里销毁自定义发送模块，防止内存泄漏
        break;
    }
    return TRUE;
}


