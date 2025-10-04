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

		// 禁止拷贝
		Logitech(const Logitech&) = delete;
		Logitech& operator=(const Logitech&) = delete;

		// 支持移动语义
		Logitech(Logitech&&) = delete;
		Logitech& operator=(Logitech&&) = delete;

	private:
		Logitech();
		~Logitech();

	public:
		static Logitech& getLogitechInstance();
		bool send_keyboard_report(const KEYBDINPUT& ki);
		bool send_mouse_report(const MOUSEINPUT& mi);

	};

} // namespace Send::Internal

