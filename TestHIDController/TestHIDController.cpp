#include <Windows.h>
#include <iostream>
#include "../include/HIDController.hpp"

/*
1.将 HIDController 项目中的 HIDController.hpp 、HIDController.dll、HIDController.lib 复制到自己项目文件夹中
2.设置 HIDController.dll 文件属性，从“不参与生成”改为“复制文件”
3.在自己项目中包含头文件 #include "HIDController.hpp"
4.启动 LGS 9.02 软件后，正常使用 HIDController.hpp 文件中的接口函数即可
*/

POINT GetMousePosition() {
	POINT pt;
	if (GetCursorPos(&pt)) {
		return pt;
	}
	else {
		pt.x = pt.y = -1;
		return pt;
	}
}

// 测试鼠标功能
void TestMouseMoveFunctions() {
	std::cout << "开始测试鼠标移动功能..." << std::endl;
	Sleep(1000);

	// 设置鼠标移动速率系数
	SetMouseMoveCoefficient(1.5f);
	Sleep(500);

	// 自动校准
	AutoCalibrate();
	Sleep(1000);

	// 禁用鼠标加速
	DisableMouseAcceleration();
	Sleep(500);

	// 相对移动鼠标
	POINT before = GetMousePosition();
	MouseMoveRelative(500, 0);
	Sleep(1000);
	POINT after = GetMousePosition();
	std::cout << "相对移动差值: (" << (after.x - (before.x + 500))
		<< "," << (after.y - before.y) << ")" << std::endl;

	// 绝对移动鼠标到屏幕中心
	int targetX = 960, targetY = 540;
	MouseMoveAbsolute(targetX, targetY);
	Sleep(1000);
	POINT current = GetMousePosition();
	std::cout << "绝对移动差值: (" << (current.x - targetX)
		<< "," << (current.y - targetY) << ")" << std::endl;

	// 启用鼠标加速
	EnableMouseAcceleration();
	Sleep(500);

	std::cout << "鼠标功能测试完成!" << std::endl;
}

void TestMouseClickFunctions() {
	std::cout << "开始测试鼠标点击功能..." << std::endl;
	Sleep(2000);

	//鼠标点击操作
	std::cout << "鼠标左键点击..." << std::endl;
	MouseClick(MOUSEEVENTF_LEFTDOWN);
	Sleep(1000);

	std::cout << "鼠标右键点击..." << std::endl;
	MouseClick(MOUSEEVENTF_RIGHTDOWN);
	Sleep(1000);

	std::cout << "鼠标中键点击..." << std::endl;
	MouseClick(MOUSEEVENTF_MIDDLEDOWN);
	Sleep(1000);

	std::cout << "鼠标 XButton1 点击..." << std::endl;
	MouseClick(MOUSEEVENTF_XDOWN | XBUTTON1);
	Sleep(1000);

	std::cout << "鼠标 XButton2 点击..." << std::endl;
	MouseClick(MOUSEEVENTF_XDOWN | XBUTTON2);  // 自动按下并松开
	Sleep(1000);

	//鼠标滚轮
	std::cout << "鼠标向上滚动一个标准单位..." << std::endl;
	MouseWheel(120);
	Sleep(1000);

	std::cout << "鼠标向下滚动两个标准单位..." << std::endl;
	MouseWheel(-240);
	Sleep(1000);
}

void TestMouseDownFunctions() {
	std::cout << "开始测试鼠标按下功能..." << std::endl;
	Sleep(1000);

	// 先松开所有按键，确保设备状态一致
	MouseUp(MOUSEEVENTF_LEFTUP);
	Sleep(1000);
	MouseUp(MOUSEEVENTF_RIGHTUP);
	Sleep(1000);
	MouseUp(MOUSEEVENTF_MIDDLEUP);
	Sleep(1000);
	MouseUp(MOUSEEVENTF_XUP | XBUTTON1);
	Sleep(1000);
	MouseUp(MOUSEEVENTF_XUP | XBUTTON2);
	Sleep(1000);

	// 按下鼠标左键
	std::cout << "按下鼠标左键..." << std::endl;
	MouseDown(MOUSEEVENTF_LEFTDOWN);
	Sleep(1000);


	// 按下鼠标右键
	std::cout << "按下鼠标右键..." << std::endl;
	MouseDown(MOUSEEVENTF_RIGHTDOWN);
	Sleep(1000);


	// 按下鼠标中键
	std::cout << "按下鼠标中键..." << std::endl;
	MouseDown(MOUSEEVENTF_MIDDLEDOWN);
	Sleep(1000);


	// 按下鼠标 XButton1
	std::cout << "按下鼠标 XButton1..." << std::endl;
	MouseDown(MOUSEEVENTF_XDOWN | XBUTTON1);
	Sleep(1000);


	// 按下鼠标 XButton2
	std::cout << "按下鼠标 XButton2..." << std::endl;
	MouseDown(MOUSEEVENTF_XDOWN | XBUTTON2);
	Sleep(1000);

	MouseUp(MOUSEEVENTF_LEFTUP);
	Sleep(1000);
	MouseUp(MOUSEEVENTF_RIGHTUP);
	Sleep(1000);
	MouseUp(MOUSEEVENTF_MIDDLEUP);
	Sleep(1000);
	MouseUp(MOUSEEVENTF_XUP | XBUTTON1);
	Sleep(1000);
	MouseUp(MOUSEEVENTF_XUP | XBUTTON2);
	Sleep(1000);
	std::cout << "鼠标按下功能测试完成!" << std::endl;
}


// 测试键盘功能
void TestKeyboardFunctions() {
	std::cout << "开始测试键盘功能..." << std::endl;

	Sleep(2000);
	// 单个按键操作
	std::cout << "按下 'A' 键..." << std::endl;
	KeyDown('A');
	Sleep(500);

	std::cout << "松开 'A' 键..." << std::endl;
	KeyUp('A');
	Sleep(500);

	// 单键点击
	std::cout << "按下 Enter 键..." << std::endl;
	KeyPress(VK_RETURN);
	Sleep(500);

	std::cout << "按下 Shift 键..." << std::endl;
	KeyPress(VK_SHIFT);
	Sleep(500);

	std::cout << "按下 Alt 键..." << std::endl;
	KeyPress(VK_MENU);
	Sleep(500);

	std::cout << "按下 Control 键..." << std::endl;
	KeyPress(VK_CONTROL);
	Sleep(500);

	std::cout << "按下 Esc 键..." << std::endl;
	KeyPress(VK_ESCAPE);
	Sleep(500);

	std::cout << "按下 F1 键..." << std::endl;
	KeyPress(VK_F1);
	Sleep(500);




	std::cout << "键盘功能测试完成!" << std::endl;
}

void TestKeyComboFunctions() {
	std::cout << "开始测试键盘组合按键功能..." << std::endl;
	Sleep(2000);
	// 组合键操作
	std::cout << "按下 Shift + Alt + T + 'A' 组合键..." << std::endl;
	KeyCombo({ VK_LSHIFT, VK_MENU, 'T' , 'A' });

	std::cout << "组合键功能测试完成!" << std::endl;
}

void TestKeySequenceFunctions() {
	std::cout << "开始测试键盘序列按键功能..." << std::endl;
	Sleep(2000);
	// 序列键操作
	std::cout << "按下拼音 'ZUHEANJIAN' 序列键..." << std::endl;
	KeySeq({ 'Z', 'U', 'H', 'E', 'A', 'N', 'J', 'I', 'A', 'N' });
}

bool IsKeyDownAsync(int vkey) {
	SHORT s = ::GetAsyncKeyState(vkey);
	return (s & 0x8000) != 0;
}

// 测试按键状态检测功能
void TestKeyStateFunctions() {
	const int N = 1000;
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);

	double total_us = 0.0;

	for (int i = 0; i < N; ++i) {
		QueryPerformanceCounter(&start);
		// 测量函数调用
		volatile bool down = IsKeyDownAsync('A');
		QueryPerformanceCounter(&end);

		double elapsed_us = (end.QuadPart - start.QuadPart) * 1'000'000.0 / freq.QuadPart;
		total_us += elapsed_us;
	}

	double avg_us = total_us / N;
	std::cout << "IsKeyDownAsync 平均耗时: " << avg_us << " 微秒 (" << N << " 次)\n";

}

void TestContinuousMouseClicks() {
	std::cout << "开始测试连续鼠标点击功能..." << std::endl;
	for (int i = 0; i < 100; ++i) {
		MouseMoveRelative(0, 0);
		MouseClick(MOUSEEVENTF_LEFTDOWN);
		Sleep(10);
	}
}

int main() {
	std::cout << "==== 开始测试 DriverMouse 功能 ====" << std::endl;
	Sleep(2000);

	//TestMouseMoveFunctions();
	//TestMouseClickFunctions();
	//TestMouseDownFunctions();

	TestContinuousMouseClicks();

	//TestKeyboardFunctions();
	//TestKeyComboFunctions();
	//TestKeySequenceFunctions();
	//TestKeyStateFunctions();

	std::cout << "==== DriverMouse 测试完成 ====" << std::endl;
	return 0;
}