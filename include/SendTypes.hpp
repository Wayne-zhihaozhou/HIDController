// SendTypes.hpp
#pragma once

//-------------------- 定义DLL导出宏 ----------
#ifdef DLL1_EXPORTS
#define DLLAPI  extern "C" __declspec(dllexport)//C语言链接方式;函数导出到DLL中
#else
#define DLLAPI  extern "C" __declspec(dllimport)//C语言链接方式;函数导入到DLL中
#endif


namespace Send {


    enum class Error : uint32_t {
        Success,
        InvalidArgument,
        LibraryNotFound,
        LibraryLoadFailed,
        LibraryError,
        DeviceCreateFailed,
        DeviceNotFound,
        DeviceOpenFailed
    };

    enum class SendType : uint32_t {
        AnyDriver = 0,
        SendInput = 1,
        Logitech = 2,
        LogitechGHubNew = 6,
        Razer = 3,
        DD = 4,
        MouClassInputInjection = 5
    };

    using InitFlags = const uint32_t;

    struct Init {
        using T = InitFlags;
    };

    enum class HookCode : uint32_t {
        Off,
        On,
        InitOnly,
        Destroy
    };


    enum class MoveMode : uint32_t {
        Absolute,  //0  // 绝对移动模式
        Relative   //1  // 相对移动模式
    };

    enum class MouseButton : uint32_t {
        LeftDown = MOUSEEVENTF_LEFTDOWN,              //0x02
        LeftUp = MOUSEEVENTF_LEFTUP,                  //0x04
        Left = LeftDown | LeftUp,                     //0x06

        RightDown = MOUSEEVENTF_RIGHTDOWN,            //0x08
        RightUp = MOUSEEVENTF_RIGHTUP,                //0x10
        Right = RightDown | RightUp,                  //0x18

        MiddleDown = MOUSEEVENTF_MIDDLEDOWN,          //0x20
        MiddleUp = MOUSEEVENTF_MIDDLEUP,              //0x40
        Middle = MiddleDown | MiddleUp,               //0x60

        XButton1Down = MOUSEEVENTF_XDOWN | XBUTTON1,  //0x81
        XButton1Up = MOUSEEVENTF_XUP | XBUTTON1,      //0x101
        XButton1 = XButton1Down | XButton1Up,         //0x181

        XButton2Down = MOUSEEVENTF_XDOWN | XBUTTON2,  //0x82
        XButton2Up = MOUSEEVENTF_XUP | XBUTTON2,      //0x102
        XButton2 = XButton2Down | XButton2Up,         //0x182
    };

    struct KeyboardModifiers {
        bool LCtrl : 1;   //0x01
        bool LShift : 1;  //0x02
        bool LAlt : 1;    //0x04
        bool LWin : 1;    //0x08
        bool RCtrl : 1;   //0x10
        bool RShift : 1;  //0x20  
        bool RAlt : 1;    //0x40
        bool RWin : 1;    //0x80
    };
}


