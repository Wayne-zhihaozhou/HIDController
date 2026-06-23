// hid_send_bindings.cpp — Python C API bindings for HIDController hid_send
// HIDController project — Send HID reports via Logitech virtual driver
#include <Python.h>
#include <windows.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "../include/hid_controller.h"

// ==================== Helper: string -> mouse button flag ====================

static uint16_t button_str_to_flag(const char* s) {
    if (strcmp(s, "xbutton1") == 0)   return MOUSEEVENTF_XDOWN | XBUTTON1;
    if (strcmp(s, "xbutton2") == 0)   return MOUSEEVENTF_XDOWN | XBUTTON2;
    if (strcmp(s, "xbutton1up") == 0) return MOUSEEVENTF_XUP | XBUTTON1;
    if (strcmp(s, "xbutton2up") == 0) return MOUSEEVENTF_XUP | XBUTTON2;
    if (strcmp(s, "left") == 0)       return 0x02;
    if (strcmp(s, "leftup") == 0)     return 0x04;
    if (strcmp(s, "right") == 0)      return 0x08;
    if (strcmp(s, "rightup") == 0)    return 0x10;
    if (strcmp(s, "middle") == 0)     return 0x20;
    if (strcmp(s, "middleup") == 0)   return 0x40;
    if (strcmp(s, "x1") == 0)         return 0x0080;
    if (strcmp(s, "x2") == 0)         return 0x0100;
    return 0;
}

// ==================== Helper: PyObject -> uint16_t (mouse button) ====================

static uint16_t py_to_button(PyObject* btn) {
    if (!PyUnicode_Check(btn)) {
        long val = PyLong_AsLong(btn);
        return (uint16_t)val;
    }
    const char* s = PyUnicode_AsUTF8(btn);
    if (!s) return 0;
    return button_str_to_flag(s);
}

// ==================== Helper: PyObject -> uint16_t (virtual key) ====================

static uint16_t key_name_to_vk(const char* s) {
    if (strcmp(s, "space") == 0)     return VK_SPACE;
    if (strcmp(s, "shift") == 0)     return VK_SHIFT;
    if (strcmp(s, "lshift") == 0)    return VK_LSHIFT;
    if (strcmp(s, "rshift") == 0)    return VK_RSHIFT;
    if (strcmp(s, "ctrl") == 0)      return VK_CONTROL;
    if (strcmp(s, "lctrl") == 0)     return VK_LCONTROL;
    if (strcmp(s, "rctrl") == 0)     return VK_RCONTROL;
    if (strcmp(s, "alt") == 0)       return VK_MENU;
    if (strcmp(s, "lalt") == 0)      return VK_LMENU;
    if (strcmp(s, "ralt") == 0)      return VK_RMENU;
    if (strcmp(s, "win") == 0)       return VK_LWIN;
    if (strcmp(s, "enter") == 0)     return VK_RETURN;
    if (strcmp(s, "escape") == 0)    return VK_ESCAPE;
    if (strcmp(s, "tab") == 0)       return VK_TAB;
    if (strcmp(s, "back") == 0)      return VK_BACK;
    if (strcmp(s, "delete") == 0)    return VK_DELETE;
    if (strcmp(s, "insert") == 0)    return VK_INSERT;
    if (strcmp(s, "home") == 0)      return VK_HOME;
    if (strcmp(s, "end") == 0)       return VK_END;
    if (strcmp(s, "pageup") == 0)    return VK_PRIOR;
    if (strcmp(s, "pagedown") == 0)  return VK_NEXT;
    if (strcmp(s, "up") == 0)        return VK_UP;
    if (strcmp(s, "down") == 0)      return VK_DOWN;
    if (strcmp(s, "left") == 0)      return VK_LEFT;
    if (strcmp(s, "right") == 0)     return VK_RIGHT;
    if (strcmp(s, "capslock") == 0)  return VK_CAPITAL;
    if (strcmp(s, "numlock") == 0)   return VK_NUMLOCK;
    if (strcmp(s, "scrolllock") == 0) return VK_SCROLL;
    if (strcmp(s, "f1") == 0)        return VK_F1;
    if (strcmp(s, "f2") == 0)        return VK_F2;
    if (strcmp(s, "f3") == 0)        return VK_F3;
    if (strcmp(s, "f4") == 0)        return VK_F4;
    if (strcmp(s, "f5") == 0)        return VK_F5;
    if (strcmp(s, "f6") == 0)        return VK_F6;
    if (strcmp(s, "f7") == 0)        return VK_F7;
    if (strcmp(s, "f8") == 0)        return VK_F8;
    if (strcmp(s, "f9") == 0)        return VK_F9;
    if (strcmp(s, "f10") == 0)       return VK_F10;
    if (strcmp(s, "f11") == 0)       return VK_F11;
    if (strcmp(s, "f12") == 0)       return VK_F12;
    return 0;
}

static uint16_t py_to_vk(PyObject* vk_obj) {
    if (PyUnicode_Check(vk_obj)) {
        Py_ssize_t len;
        const char* s = PyUnicode_AsUTF8AndSize(vk_obj, &len);
        if (!s) return 0;
        uint16_t code = key_name_to_vk(s);
        if (code) return code;
        // Single character fallback
        if (len == 1) return (uint16_t)s[0];
        return 0;
    }
    long val = PyLong_AsLong(vk_obj);
    return (uint16_t)val;
}

// ==================== Mouse functions ====================

static PyObject* hid_move_mouse_relative(PyObject* self, PyObject* args) {
    int32_t dx, dy;
    if (!PyArg_ParseTuple(args, "ii", &dx, &dy))
        return NULL;
    return PyBool_FromLong((long)mouse_move_relative(dx, dy));
}

static PyObject* hid_move_mouse_absolute(PyObject* self, PyObject* args) {
    uint32_t x, y;
    if (!PyArg_ParseTuple(args, "II", &x, &y))
        return NULL;
    return PyBool_FromLong((long)mouse_move_absolute(x, y));
}

static PyObject* hid_mouse_button_op(PyObject* self, PyObject* args, int op) {
    // op=0: down, op=1: up, op=2: click/press
    PyObject* btn_obj;
    if (!PyArg_ParseTuple(args, "O", &btn_obj))
        return NULL;

    uint16_t flag = py_to_button(btn_obj);
    bool result;

    switch (op) {
        case 0: result = mouse_down(flag); break;
        case 1: result = mouse_up(flag);   break;
        default: result = mouse_click(flag); break;
    }

    return PyBool_FromLong((long)result);
}

static PyObject* hid_mouse_down(PyObject* self, PyObject* args) {
    return hid_mouse_button_op(self, args, 0);
}

static PyObject* hid_mouse_up(PyObject* self, PyObject* args) {
    return hid_mouse_button_op(self, args, 1);
}

static PyObject* hid_mouse_click(PyObject* self, PyObject* args) {
    return hid_mouse_button_op(self, args, 2);
}

static PyObject* hid_mouse_wheel(PyObject* self, PyObject* args) {
    int32_t movement;
    if (!PyArg_ParseTuple(args, "i", &movement))
        return NULL;
    return PyBool_FromLong((long)mouse_wheel(movement));
}

static PyObject* hid_set_mouse_move_coefficient(PyObject* self, PyObject* args) {
    float coefficient;
    if (!PyArg_ParseTuple(args, "f", &coefficient))
        return NULL;
    set_mouse_move_coefficient(coefficient);
    Py_RETURN_NONE;
}

static PyObject* hid_get_mouse_move_coefficient(PyObject* self, PyObject* args) {
    float coeff = get_mouse_move_coefficient();
    return PyFloat_FromDouble((double)coeff);
}

static PyObject* hid_auto_calibrate(PyObject* self, PyObject* args) {
    auto_calibrate();
    Py_RETURN_NONE;
}

static PyObject* hid_disable_mouse_acceleration(PyObject* self, PyObject* args) {
    disable_mouse_acceleration();
    Py_RETURN_NONE;
}

static PyObject* hid_enable_mouse_acceleration(PyObject* self, PyObject* args) {
    enable_mouse_acceleration();
    Py_RETURN_NONE;
}

// ==================== Keyboard functions ====================

static PyObject* hid_key_op(PyObject* self, PyObject* args, int op) {
    // op=0: down, op=1: up, op=2: press
    PyObject* vk_obj;
    if (!PyArg_ParseTuple(args, "O", &vk_obj))
        return NULL;

    uint16_t vk = py_to_vk(vk_obj);
    bool result;

    switch (op) {
        case 0: result = key_down(vk);  break;
        case 1: result = key_up(vk);    break;
        default: result = key_press(vk); break;
    }

    return PyBool_FromLong((long)result);
}

static PyObject* hid_key_down(PyObject* self, PyObject* args) {
    return hid_key_op(self, args, 0);
}

static PyObject* hid_key_up(PyObject* self, PyObject* args) {
    return hid_key_op(self, args, 1);
}

static PyObject* hid_key_press(PyObject* self, PyObject* args) {
    return hid_key_op(self, args, 2);
}

// ==================== key_combo and key_seq ====================

static PyObject* hid_key_sequence_op(PyObject* self, PyObject* args, bool sequential) {
    PyObject* keys_list;
    if (!PyArg_ParseTuple(args, "O", &keys_list))
        return NULL;
    if (!PyList_Check(keys_list)) {
        PyErr_SetString(PyExc_TypeError, "keys must be a list");
        return NULL;
    }

    Py_ssize_t n = PyList_Size(keys_list);
    if (n < 0) return NULL;
    if (n > 256) {
        PyErr_SetString(PyExc_ValueError, "keys list too long (max 256)");
        return NULL;
    }

    std::vector<uint16_t> codes;
    codes.reserve((size_t)n);
    for (Py_ssize_t i = 0; i < n; i++) {
        PyObject* item = PyList_GetItem(keys_list, i);
        codes.push_back(py_to_vk(item));
    }

    bool result;
    if (sequential)
        result = key_seq(codes);
    else
        result = key_combo(codes);

    return PyBool_FromLong((long)result);
}

static PyObject* hid_key_combo(PyObject* self, PyObject* args) {
    return hid_key_sequence_op(self, args, false);
}

static PyObject* hid_key_seq(PyObject* self, PyObject* args) {
    return hid_key_sequence_op(self, args, true);
}

static PyObject* hid_release_all_keys(PyObject* self, PyObject* args) {
    release_all_keys();
    Py_RETURN_NONE;
}

// ==================== Method table ====================

static PyMethodDef HidSendMethods[] = {
    {"mouse_move_relative",    (PyCFunction)hid_move_mouse_relative,    METH_VARARGS,
     "Move mouse relatively. Args: dx (int), dy (int)"},
    {"mouse_move_absolute",    (PyCFunction)hid_move_mouse_absolute,    METH_VARARGS,
     "Move mouse absolutely. Args: x (int), y (int)"},
    {"mouse_down",             (PyCFunction)hid_mouse_down,             METH_VARARGS,
     "Mouse button down. Args: button (int or str) - e.g., 'left', 'right', 'middle', 'xbutton1', 'xbutton2'"},
    {"mouse_up",               (PyCFunction)hid_mouse_up,               METH_VARARGS,
     "Mouse button up. Args: button (int or str)"},
    {"mouse_click",            (PyCFunction)hid_mouse_click,            METH_VARARGS,
     "Mouse click (down + up). Args: button (int or str)"},
    {"mouse_wheel",            (PyCFunction)hid_mouse_wheel,            METH_VARARGS,
     "Mouse wheel scroll. Args: movement (int) - typically 120 for one notch"},
    {"set_mouse_move_coefficient", (PyCFunction)hid_set_mouse_move_coefficient, METH_VARARGS,
     "Set mouse move speed coefficient. Args: coefficient (float)"},
    {"get_mouse_move_coefficient", (PyCFunction)hid_get_mouse_move_coefficient, METH_NOARGS,
     "Get current mouse move speed coefficient (float)."},
    {"auto_calibrate",         (PyCFunction)hid_auto_calibrate,         METH_NOARGS,
     "Automatically calibrate mouse speed coefficient."},
    {"disable_mouse_acceleration", (PyCFunction)hid_disable_mouse_acceleration, METH_NOARGS,
     "Disable Windows mouse acceleration."},
    {"enable_mouse_acceleration",  (PyCFunction)hid_enable_mouse_acceleration,  METH_NOARGS,
     "Restore Windows mouse acceleration."},
    {"key_down",               (PyCFunction)hid_key_down,               METH_VARARGS,
     "Key down. Args: vk (int or str) - virtual key code or key character/string"},
    {"key_up",                 (PyCFunction)hid_key_up,                 METH_VARARGS,
     "Key up. Args: vk (int or str)"},
    {"key_press",              (PyCFunction)hid_key_press,              METH_VARARGS,
     "Key press (down + up). Args: vk (int or str)"},
    {"key_combo",              (PyCFunction)hid_key_combo,              METH_VARARGS,
     "Key combination (press all then release all in reverse). Args: keys (list of int or str)"},
    {"key_seq",                (PyCFunction)hid_key_seq,                METH_VARARGS,
     "Key sequence (press and release each key in order). Args: keys (list of int or str)"},
    {"release_all_keys",       (PyCFunction)hid_release_all_keys,       METH_NOARGS,
     "Release all pressed keys."},
    {NULL, NULL, 0, NULL}
};

// ==================== Module definition ====================

static struct PyModuleDef hid_send_module = {
    PyModuleDef_HEAD_INIT,
    "hid_send",
    "HIDController Python bindings - Mouse and keyboard control via Logitech HID reports",
    -1,
    HidSendMethods
};

PyMODINIT_FUNC PyInit_hid_send(void) {
    return PyModule_Create(&hid_send_module);
}
