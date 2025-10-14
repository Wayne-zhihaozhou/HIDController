#include <Windows.h>
#include <iostream>
#include "HIDController.hpp"


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

// ������깦��
void TestMouseMoveFunctions() {
	std::cout << "��ʼ��������ƶ�����..." << std::endl;
	Sleep(1000);

	// ��������ƶ�����ϵ��
	SetMouseMoveCoefficient(1.5f);
	Sleep(500);

	// �Զ�У׼
	AutoCalibrate();
	Sleep(1000);

	// ����������
	DisableMouseAcceleration();
	Sleep(500);

	// ����ƶ����
	POINT before = GetMousePosition();
	MouseMoveRelative(500, 0);
	Sleep(1000);
	POINT after = GetMousePosition();
	std::cout << "����ƶ���ֵ: (" << (after.x - (before.x + 500))
		<< "," << (after.y - before.y) << ")" << std::endl;

	// �����ƶ���굽��Ļ����
	int targetX = 960, targetY = 540;
	MouseMoveAbsolute(targetX, targetY);
	Sleep(1000);
	POINT current = GetMousePosition();
	std::cout << "�����ƶ���ֵ: (" << (current.x - targetX)
		<< "," << (current.y - targetY) << ")" << std::endl;

	// ����������
	EnableMouseAcceleration();
	Sleep(500);

	std::cout << "��깦�ܲ������!" << std::endl;
}

void TestMouseClickFunctions() {
	std::cout << "��ʼ�������������..." << std::endl;
	Sleep(2000);

	//���������
	std::cout << "���������..." << std::endl;
	MouseClick(MOUSEEVENTF_LEFTDOWN);
	Sleep(1000);

	std::cout << "����Ҽ����..." << std::endl;
	MouseClick(MOUSEEVENTF_RIGHTDOWN);
	Sleep(1000);

	std::cout << "����м����..." << std::endl;
	MouseClick(MOUSEEVENTF_MIDDLEDOWN);
	Sleep(1000);

	std::cout << "��� XButton1 ���..." << std::endl;
	MouseClick(MOUSEEVENTF_XDOWN | XBUTTON1);
	Sleep(1000);

	std::cout << "��� XButton2 ���..." << std::endl;
	MouseClick(MOUSEEVENTF_XDOWN | XBUTTON2);  // �Զ����²��ɿ�
	Sleep(1000);

	//������
	std::cout << "������Ϲ���һ����׼��λ..." << std::endl;
	MouseWheel(120);
	Sleep(1000);

	std::cout << "������¹���������׼��λ..." << std::endl;
	MouseWheel(-240);
	Sleep(1000);
}

void TestMouseDownFunctions() {
	std::cout << "��ʼ������갴�¹���..." << std::endl;
	Sleep(1000);

	// ���ɿ����а�����ȷ���豸״̬һ��
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

	// ����������
	std::cout << "����������..." << std::endl;
	MouseDown(MOUSEEVENTF_LEFTDOWN);
	Sleep(1000);


	// ��������Ҽ�
	std::cout << "��������Ҽ�..." << std::endl;
	MouseDown(MOUSEEVENTF_RIGHTDOWN);
	Sleep(1000);


	// ��������м�
	std::cout << "��������м�..." << std::endl;
	MouseDown(MOUSEEVENTF_MIDDLEDOWN);
	Sleep(1000);


	// ������� XButton1
	std::cout << "������� XButton1..." << std::endl;
	MouseDown(MOUSEEVENTF_XDOWN | XBUTTON1);
	Sleep(1000);


	// ������� XButton2
	std::cout << "������� XButton2..." << std::endl;
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
	std::cout << "��갴�¹��ܲ������!" << std::endl;
}



// ���Լ��̹���
void TestKeyboardFunctions() {
	std::cout << "��ʼ���Լ��̹���..." << std::endl;

	Sleep(2000);
	// ������������
	std::cout << "���� 'A' ��..." << std::endl;
	KeyDown('A');
	Sleep(500);

	std::cout << "�ɿ� 'A' ��..." << std::endl;
	KeyUp('A');
	Sleep(500);

	// �������
	std::cout << "���� Enter ��..." << std::endl;
	KeyPress(VK_RETURN);
	Sleep(500);

	std::cout << "���� Shift ��..." << std::endl;
	KeyPress(VK_SHIFT);
	Sleep(500);

	std::cout << "���� Alt ��..." << std::endl;
	KeyPress(VK_MENU);
	Sleep(500);

	std::cout << "���� Control ��..." << std::endl;
	KeyPress(VK_CONTROL);
	Sleep(500);

	std::cout << "���� Esc ��..." << std::endl;
	KeyPress(VK_ESCAPE);
	Sleep(500);

	std::cout << "���� F1 ��..." << std::endl;
	KeyPress(VK_F1);
	Sleep(500);




	std::cout << "���̹��ܲ������!" << std::endl;
}

void TestKeyComboFunctions() {
	std::cout << "��ʼ���Լ�����ϰ�������..." << std::endl;
	Sleep(2000);
	// ��ϼ�����
	std::cout << "���� Shift + Alt + T + 'A' ��ϼ�..." << std::endl;
	KeyCombo({ VK_LSHIFT, VK_MENU, 'T' , 'A' });

	std::cout << "��ϼ����ܲ������!" << std::endl;
}

void TestKeySequenceFunctions() {
	std::cout << "��ʼ���Լ������а�������..." << std::endl;
	Sleep(2000);
	// ���м�����
	std::cout << "����ƴ�� 'ZUHEANJIAN' ���м�..." << std::endl;
	KeySeq({ 'Z', 'U', 'H', 'E', 'A', 'N', 'J', 'I', 'A', 'N' });
}


bool IsKeyDownAsync(int vkey) {
	SHORT s = ::GetAsyncKeyState(vkey);
	return (s & 0x8000) != 0;
}

// ���԰���״̬��⹦��
void TestKeyStateFunctions() {
	const int N = 1000;
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);

	double total_us = 0.0;

	for (int i = 0; i < N; ++i) {
		QueryPerformanceCounter(&start);
		// ������������
		volatile bool down = IsKeyDownAsync('A');
		QueryPerformanceCounter(&end);

		double elapsed_us = (end.QuadPart - start.QuadPart) * 1'000'000.0 / freq.QuadPart;
		total_us += elapsed_us;
	}

	double avg_us = total_us / N;
	std::cout << "IsKeyDownAsync ƽ����ʱ: " << avg_us << " ΢�� (" << N << " ��)\n";

}

int main() {
	std::cout << "==== ��ʼ���� DriverMouse ���� ====" << std::endl;
	Sleep(2000);

	//TestMouseMoveFunctions();
	//TestMouseClickFunctions();
	//TestMouseDownFunctions();
	//TestKeyboardFunctions();
	//TestKeyComboFunctions();
	//TestKeySequenceFunctions();
	TestKeyStateFunctions();

	std::cout << "==== DriverMouse ������� ====" << std::endl;
	return 0;
}