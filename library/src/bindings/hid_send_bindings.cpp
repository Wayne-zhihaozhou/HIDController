#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <windows.h>
#include "hid_controller.h"

namespace py = pybind11;

/*
 * HIDController Python 扩展
 *
 * 通过向 Logitech 虚拟驱动发送 HID 报告来控制键盘鼠标。
 * 使用前必须安装并启动 Logitech Gaming Software (LGS)。
 */

// ==================== 鼠标按钮字符串映射 ====================

/**
 * 将字符串按钮名转换为鼠标按钮常量
 * 支持: "left", "right", "middle", "x1", "x2", "wheel"
 */
static uint16_t mouse_button_to_flag(const std::string& btn) {
    if (btn == "left")      return 0x02;  // MOUSEEVENTF_LEFTDOWN
    if (btn == "leftup")    return 0x04;  // MOUSEEVENTF_LEFTUP
    if (btn == "right")     return 0x08;  // MOUSEEVENTF_RIGHTDOWN
    if (btn == "rightup")   return 0x10;  // MOUSEEVENTF_RIGHTUP
    if (btn == "middle")    return 0x20;  // MOUSEEVENTF_MIDDLEDOWN
    if (btn == "middleup")  return 0x40;  // MOUSEEVENTF_MIDDLEUP
    if (btn == "x1")        return 0x0080; // MOUSEEVENTF_XDOWN
    if (btn == "x2")        return 0x0100; // MOUSEEVENTF_XUP (actually XUP)
    if (btn == "xbutton1")  return 0x0080; // XDOWN for XBUTTON1
    if (btn == "xbutton2")  return 0x0100; // XUP for XBUTTON2
    return 0;
}

/**
 * 获取 X 按钮的参数值
 */
static uint16_t mouse_x_button_param(const std::string& btn) {
    if (btn == "xbutton1") return 1;
    if (btn == "xbutton2") return 2;
    return 0;
}

// ==================== 辅助函数 ====================

/**
 * 将字符串键名转换为虚拟键码
 */
static uint16_t key_to_vk(const std::string& key) {
    // 特殊修饰键
    if (key == "space")    return VK_SPACE;
    if (key == "shift")    return VK_SHIFT;
    if (key == "lshift")   return VK_LSHIFT;
    if (key == "rshift")   return VK_RSHIFT;
    if (key == "ctrl")     return VK_CONTROL;
    if (key == "lctrl")    return VK_LCONTROL;
    if (key == "rctrl")    return VK_RCONTROL;
    if (key == "alt")      return VK_MENU;
    if (key == "lalt")     return VK_LMENU;
    if (key == "ralt")     return VK_RMENU;
    if (key == "win")      return VK_LWIN;
    if (key == "enter")    return VK_RETURN;
    if (key == "escape")   return VK_ESCAPE;
    if (key == "tab")      return VK_TAB;
    if (key == "back")     return VK_BACK;
    if (key == "delete")   return VK_DELETE;
    if (key == "insert")   return VK_INSERT;
    if (key == "home")     return VK_HOME;
    if (key == "end")      return VK_END;
    if (key == "pageup")   return VK_PRIOR;
    if (key == "pagedown") return VK_NEXT;
    if (key == "up")       return VK_UP;
    if (key == "down")     return VK_DOWN;
    if (key == "left")     return VK_LEFT;
    if (key == "right")    return VK_RIGHT;
    if (key == "capslock") return VK_CAPITAL;
    if (key == "numlock")  return VK_NUMLOCK;
    if (key == "scrolllock") return VK_SCROLL;

    // 功能键
    if (key == "f1") return VK_F1;
    if (key == "f2") return VK_F2;
    if (key == "f3") return VK_F3;
    if (key == "f4") return VK_F4;
    if (key == "f5") return VK_F5;
    if (key == "f6") return VK_F6;
    if (key == "f7") return VK_F7;
    if (key == "f8") return VK_F8;
    if (key == "f9") return VK_F9;
    if (key == "f10") return VK_F10;
    if (key == "f11") return VK_F11;
    if (key == "f12") return VK_F12;

    // 单字符处理
    if (key.length() == 1) {
        char c = key[0];
        // 字母 A-Z
        if (c >= 'a' && c <= 'z') return static_cast<uint16_t>(c);
        if (c >= 'A' && c <= 'Z') return static_cast<uint16_t>(c);
        // 数字 0-9
        if (c >= '0' && c <= '9') return static_cast<uint16_t>(c);
        // 其他字符
        return static_cast<uint16_t>(c);
    }

    // 默认返回 0
    return 0;
}

PYBIND11_MODULE(hid_send, m) {
    m.doc() = "Python bindings for HIDController - Mouse and keyboard control via Logitech HID reports";

    // ==================== 鼠标控制函数 ====================

    m.def("move_mouse_relative", &mouse_move_relative,
          py::arg("dx"), py::arg("dy"),
          "Move mouse relatively. Args: dx (int), dy (int)");

    m.def("move_mouse_absolute", &mouse_move_absolute,
          py::arg("x"), py::arg("y"),
          "Move mouse absolutely. Args: x (int), y (int)");

    m.def("mouse_down", [](py::object btn) -> bool {
        uint16_t button_flag = 0;

        if (py::isinstance<py::str>(btn)) {
            std::string button_str = btn.cast<std::string>();
            button_flag = mouse_button_to_flag(button_str);
            // XButton 需要将 XBUTTON1/XBUTTON2 位合并到 button_flag 中
            if (button_str == "xbutton1") {
                button_flag = MOUSEEVENTF_XDOWN | XBUTTON1;
            } else if (button_str == "xbutton2") {
                button_flag = MOUSEEVENTF_XDOWN | XBUTTON2;
            }
        } else {
            button_flag = btn.cast<uint16_t>();
        }

        return mouse_down(button_flag);
    }, py::arg("button"),
       "Mouse button down. Args: button (int or str) - e.g., 'left', 'right', 'middle', 'xbutton1', 'xbutton2'");

    m.def("mouse_up", [](py::object btn) -> bool {
        uint16_t button_flag = 0;

        if (py::isinstance<py::str>(btn)) {
            std::string button_str = btn.cast<std::string>();
            button_flag = mouse_button_to_flag(button_str);
            // XButton 需要将 XBUTTON1/XBUTTON2 位合并到 button_flag 中
            if (button_str == "xbutton1") {
                button_flag = MOUSEEVENTF_XUP | XBUTTON1;
            } else if (button_str == "xbutton2") {
                button_flag = MOUSEEVENTF_XUP | XBUTTON2;
            }
        } else {
            button_flag = btn.cast<uint16_t>();
        }

        return mouse_up(button_flag);
    }, py::arg("button"),
       "Mouse button up. Args: button (int or str)");

    m.def("mouse_press", [](py::object btn) -> bool {
        uint16_t button_flag = 0;

        if (py::isinstance<py::str>(btn)) {
            std::string button_str = btn.cast<std::string>();
            button_flag = mouse_button_to_flag(button_str);
            // XButton 需要将 XBUTTON1/XBUTTON2 位合并到 button_flag 中
            if (button_str == "xbutton1") {
                button_flag = MOUSEEVENTF_XDOWN | XBUTTON1;
            } else if (button_str == "xbutton2") {
                button_flag = MOUSEEVENTF_XDOWN | XBUTTON2;
            }
        } else {
            button_flag = btn.cast<uint16_t>();
        }

        return mouse_click(button_flag);
    }, py::arg("button"),
       "Mouse press (down + up). Args: button (int or str) - e.g., 'left', 'right', 'middle'");

    m.def("mouse_click", [](py::object btn) -> bool {
        uint16_t button_flag = 0;

        if (py::isinstance<py::str>(btn)) {
            std::string button_str = btn.cast<std::string>();
            button_flag = mouse_button_to_flag(button_str);
            // XButton 需要将 XBUTTON1/XBUTTON2 位合并到 button_flag 中
            if (button_str == "xbutton1") {
                button_flag = MOUSEEVENTF_XDOWN | XBUTTON1;
            } else if (button_str == "xbutton2") {
                button_flag = MOUSEEVENTF_XDOWN | XBUTTON2;
            }
        } else {
            button_flag = btn.cast<uint16_t>();
        }

        return mouse_click(button_flag);
    }, py::arg("button"),
       "Mouse click (down + up). Args: button (int or str)");

    m.def("mouse_wheel", &mouse_wheel,
          py::arg("movement"),
          "Mouse wheel scroll. Args: movement (int) - typically 120 for one notch (positive=up, negative=down)");

    m.def("set_mouse_move_coefficient", &set_mouse_move_coefficient,
          py::arg("coefficient"),
          "Set mouse move speed coefficient. Args: coefficient (float)");

    m.def("auto_calibrate", &auto_calibrate,
          "Automatically calibrate mouse speed coefficient.");

    m.def("disable_mouse_acceleration", &disable_mouse_acceleration,
          "Disable Windows mouse acceleration.");

    m.def("enable_mouse_acceleration", &enable_mouse_acceleration,
          "Restore Windows mouse acceleration.");

    // ==================== 键盘控制函数 ====================

    m.def("key_down", [](py::object vk) -> bool {
        uint16_t vk_code = 0;

        if (py::isinstance<py::str>(vk)) {
            std::string key = vk.cast<std::string>();
            vk_code = key_to_vk(key);
        } else {
            vk_code = vk.cast<uint16_t>();
        }

        return key_down(vk_code);
    }, py::arg("vk"),
       "Key down. Args: vk (int or str) - virtual key code or key character/string");

    m.def("key_up", [](py::object vk) -> bool {
        uint16_t vk_code = 0;

        if (py::isinstance<py::str>(vk)) {
            std::string key = vk.cast<std::string>();
            vk_code = key_to_vk(key);
        } else {
            vk_code = vk.cast<uint16_t>();
        }

        return key_up(vk_code);
    }, py::arg("vk"),
       "Key up. Args: vk (int or str)");

    m.def("key_press", [](py::object vk) -> bool {
        uint16_t vk_code = 0;

        if (py::isinstance<py::str>(vk)) {
            std::string key = vk.cast<std::string>();
            vk_code = key_to_vk(key);
        } else {
            vk_code = vk.cast<uint16_t>();
        }

        return key_press(vk_code);
    }, py::arg("vk"),
       "Key press (down + up). Args: vk (int or str)");

    m.def("key_combo", [](py::list keys) -> bool {
        std::vector<uint16_t> vk_codes;
        for (auto item : keys) {
            uint16_t code = 0;
            if (py::isinstance<py::str>(item)) {
                std::string key = item.cast<std::string>();
                code = key_to_vk(key);
            } else {
                code = item.cast<uint16_t>();
            }
            vk_codes.push_back(code);
        }
        return key_combo(vk_codes);
    }, py::arg("keys"),
       "Key combination (press all then release all in reverse). Args: keys (list of int or str)");

    m.def("key_seq", [](py::list keys) -> bool {
        std::vector<uint16_t> vk_codes;
        for (auto item : keys) {
            uint16_t code = 0;
            if (py::isinstance<py::str>(item)) {
                std::string key = item.cast<std::string>();
                code = key_to_vk(key);
            } else {
                code = item.cast<uint16_t>();
            }
            vk_codes.push_back(code);
        }
        return key_seq(vk_codes);
    }, py::arg("keys"),
       "Key sequence (press and release each key in order). Args: keys (list of int or str)");

    m.def("release_all_keys", &release_all_keys,
          "Release all pressed keys.");
}
