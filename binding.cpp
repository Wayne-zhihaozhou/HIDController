#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <windows.h>
#include "HIDController.hpp"

namespace py = pybind11;

/*
 * HIDController Python 扩展
 *
 * 通过向 Logitech 虚拟驱动发送 HID 报告来控制键盘鼠标。
 * 使用前必须安装并启动 Logitech Gaming Software (LGS)。
 */

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

PYBIND11_MODULE(_extension, m) {
    m.doc() = "Python bindings for HIDController - Mouse and keyboard control via Logitech HID reports";

    // ==================== 鼠标控制函数 ====================

    m.def("move_mouse_relative", &MouseMoveRelative,
          py::arg("dx"), py::arg("dy"),
          "Move mouse relatively. Args: dx (int), dy (int)");

    m.def("move_mouse_absolute", &MouseMoveAbsolute,
          py::arg("x"), py::arg("y"),
          "Move mouse absolutely. Args: x (int), y (int)");

    m.def("mouse_down", &MouseDown,
          py::arg("button"),
          "Mouse button down. Args: button (int) - MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_RIGHTDOWN, etc.");

    m.def("mouse_up", &MouseUp,
          py::arg("button"),
          "Mouse button up. Args: button (int)");

    m.def("mouse_click", &MouseClick,
          py::arg("button"),
          "Mouse click (down + up). Args: button (int)");

    m.def("mouse_wheel", &MouseWheel,
          py::arg("movement"),
          "Mouse wheel scroll. Args: movement (int) - typically 120 for one notch");

    m.def("set_mouse_move_coefficient", &SetMouseMoveCoefficient,
          py::arg("coefficient"),
          "Set mouse move speed coefficient. Args: coefficient (float)");

    m.def("auto_calibrate", &AutoCalibrate,
          "Automatically calibrate mouse speed coefficient.");

    m.def("disable_mouse_acceleration", &DisableMouseAcceleration,
          "Disable Windows mouse acceleration.");

    m.def("enable_mouse_acceleration", &EnableMouseAcceleration,
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

        return KeyDown(vk_code);
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

        return KeyUp(vk_code);
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

        return KeyPress(vk_code);
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
        return KeyCombo(vk_codes);
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
        return KeySeq(vk_codes);
    }, py::arg("keys"),
       "Key sequence (press and release each key in order). Args: keys (list of int or str)");

    m.def("release_all_keys", &release_all_keys,
          "Release all pressed keys.");
}
