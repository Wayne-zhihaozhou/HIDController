//Logitech.hpp
#pragma once
#include <LogitechDriver.hpp>

//-------------------- 定义DLL导出宏 ----------
#ifdef DLL1_EXPORTS
#define DLLAPI  extern "C" __declspec(dllexport)//C语言链接方式;函数导出到DLL中
#else
#define DLLAPI  extern "C" __declspec(dllimport)//C语言链接方式;函数导入到DLL中
#endif


namespace Send::Internal {

	class Logitech {
	private:
		LogitechDriver driver{};
		std::mutex mouse_mutex;
		std::mutex keyboard_mutex;
	public:
		static Logitech& getLogitechInstance();

	public:
		Logitech();
		bool create();
		void destroy();
		bool send_keyboard_report(const KEYBDINPUT& ki);
		bool send_mouse_report(const MOUSEINPUT& mi);

	};

} // namespace Send::Internal

