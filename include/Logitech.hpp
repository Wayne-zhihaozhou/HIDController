//Logitech.hpp
#pragma once
#include "SendTypes.hpp"
#include "LogitechDriver.hpp"

//-------------------- 定义DLL导出宏 ----------
#ifdef DLL1_EXPORTS
#define DLLAPI  extern "C" __declspec(dllexport)//C语言链接方式;函数导出到DLL中
#else
#define DLLAPI  extern "C" __declspec(dllimport)//C语言链接方式;函数导入到DLL中
#endif


namespace Send::Internal {

	class Logitech : public Send::Internal::Base {
	private:
		LogitechDriver driver{};
		std::mutex mouse_mutex;
		std::mutex keyboard_mutex;

	public:
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

	public:
		Logitech();
		bool create();
		void destroy();
		bool send_keyboard_report(const KEYBDINPUT& ki);
		bool send_mouse_report(const MOUSEINPUT& mi);

	};

} // namespace Send::Internal

