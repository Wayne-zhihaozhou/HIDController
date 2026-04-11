#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <windows.h>

// 直接使用Windows API，避免Logitech类依赖
namespace py = pybind11;

PYBIND11_MODULE(hid_controller, m) {
    m.doc() = "Python bindings for HIDController using direct Windows API";

    // Mouse control functions
    m.def("MouseMoveRelative", [](int32_t dx, int32_t dy) {
        // 使用SendInput直接发送鼠标移动
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        input.mi.dx = dx;
        input.mi.dy = dy;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        return SendInput(1, &input, sizeof(INPUT)) > 0;
    }, "Move mouse relatively");

    m.def("MouseMoveAbsolute", [](uint32_t x, uint32_t y) {
        // 使用SendInput直接发送绝对鼠标位置
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        input.mi.dx = x;
        input.mi.dy = y;
        input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
        return SendInput(1, &input, sizeof(INPUT)) > 0;
    }, "Move mouse absolutely");

    m.def("MouseDown", [](uint16_t button) {
        // 模拟鼠标按下
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        if (button & 0x01) input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;   // 左键
        if (button & 0x02) input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;  // 右键
        if (button & 0x04) input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN; // 中键
        if (button & 0x08) input.mi.dwFlags |= MOUSEEVENTF_XDOWN;      // X键
        return SendInput(1, &input, sizeof(INPUT)) > 0;
    }, "Mouse down");

    m.def("MouseUp", [](uint16_t button) {
        // 模拟鼠标抬起
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        if (button & 0x01) input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;   // 左键
        if (button & 0x02) input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;  // 右键
        if (button & 0x04) input.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP; // 中键
        if (button & 0x08) input.mi.dwFlags |= MOUSEEVENTF_XUP;      // X键
        return SendInput(1, &input, sizeof(INPUT)) > 0;
    }, "Mouse up");

    m.def("MouseClick", [](uint16_t button) {
        // 模拟鼠标点击（按下然后抬起）
        // 直接使用SendInput，避免调用未定义的函数
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        if (button & 0x01) input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;   // 左键按下
        if (button & 0x02) input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;  // 右键按下
        if (button & 0x04) input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN; // 中键按下
        if (button & 0x08) input.mi.dwFlags |= MOUSEEVENTF_XDOWN;      // X键按下
        SendInput(1, &input, sizeof(INPUT));
        
        // 再抬起
        input.mi.dwFlags = 0;
        if (button & 0x01) input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;   // 左键抬起
        if (button & 0x02) input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;  // 右键抬起
        if (button & 0x04) input.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP; // 中键抬起
        if (button & 0x08) input.mi.dwFlags |= MOUSEEVENTF_XUP;      // X键抬起
        return SendInput(1, &input, sizeof(INPUT)) > 0;
    }, "Mouse click");

    m.def("MouseWheel", [](int32_t movement) {
        // 模拟鼠标滚轮
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_WHEEL;
        input.mi.mouseData = movement;
        return SendInput(1, &input, sizeof(INPUT)) > 0;
    }, "Mouse wheel");

    // Keyboard control functions
    m.def("KeyDown", [](uint16_t vk) {
        // 模拟按键按下
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vk;
        input.ki.dwFlags = 0;  // 按下
        return SendInput(1, &input, sizeof(INPUT)) > 0;
    }, "Key down");

    m.def("KeyUp", [](uint16_t vk) {
        // 模拟按键抬起
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vk;
        input.ki.dwFlags = KEYEVENTF_KEYUP;  // 抬起
        return SendInput(1, &input, sizeof(INPUT)) > 0;
    }, "Key up");

    m.def("KeyPress", [](uint16_t vk) {
        // 模拟按键按下和抬起
        // 直接使用SendInput，避免调用未定义的函数
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vk;
        input.ki.dwFlags = 0;  // 按下
        SendInput(1, &input, sizeof(INPUT));
        
        input.ki.dwFlags = KEYEVENTF_KEYUP;  // 抬起
        return SendInput(1, &input, sizeof(INPUT)) > 0;
    }, "Key press");

    // 为 KeyCombo 和 KeySeq 使用特殊处理
    m.def("KeyCombo", [](py::list keys) {
        // 模拟组合键
        std::vector<INPUT> inputs;
        inputs.reserve(py::len(keys) * 2);

        // 先按下所有键
        for (auto vk : keys) {
            uint16_t vk_code = vk.cast<uint16_t>();
            INPUT input = {0};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = vk_code;
            input.ki.dwFlags = 0;  // 按下
            inputs.push_back(input);
        }

        // 再抬起所有键
        for (auto it = keys.end(); it != keys.begin();) {
            --it;
            uint16_t vk_code = it->cast<uint16_t>();
            INPUT input = {0};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = vk_code;
            input.ki.dwFlags = KEYEVENTF_KEYUP;  // 抬起
            inputs.push_back(input);
        }

        return SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT)) > 0;
    }, "Key combination");

    m.def("KeySeq", [](py::list keys) {
        // 模拟按键序列
        std::vector<INPUT> inputs;
        inputs.reserve(py::len(keys) * 2);

        for (auto vk : keys) {
            uint16_t vk_code = vk.cast<uint16_t>();
            // 按下
            INPUT inputDown = {0};
            inputDown.type = INPUT_KEYBOARD;
            inputDown.ki.wVk = vk_code;
            inputDown.ki.dwFlags = 0;
            inputs.push_back(inputDown);

            // 抬起
            INPUT inputUp = {0};
            inputUp.type = INPUT_KEYBOARD;
            inputUp.ki.wVk = vk_code;
            inputUp.ki.dwFlags = KEYEVENTF_KEYUP;
            inputs.push_back(inputUp);
        }

        return SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT)) > 0;
    }, "Key sequence");

    m.def("release_all_keys", []() {
        // 释放所有按键（这个功能比较复杂，暂时不实现）
        return true;
    }, "Release all keys");
}
