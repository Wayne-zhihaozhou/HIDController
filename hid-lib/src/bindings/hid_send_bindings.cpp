// hid_send_bindings.cpp — Python C API bindings for HIDController hid_send
// HIDController project — Send HID reports via Logitech virtual driver
#include <Python.h>
#include <windows.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "../include/hid_controller.h"

// ==================== Helper: PyObject -> MouseButton ====================

static MouseButton py_to_mouse_button(PyObject* obj) {
    if (PyUnicode_Check(obj)) {
        const char* s = PyUnicode_AsUTF8(obj);
        if (!s) return MouseButton::LEFT;
        if (strcmp(s, "left") == 0)       return MouseButton::LEFT;
        if (strcmp(s, "right") == 0)      return MouseButton::RIGHT;
        if (strcmp(s, "middle") == 0)     return MouseButton::MIDDLE;
        if (strcmp(s, "xbutton1") == 0)   return MouseButton::X1;
        if (strcmp(s, "xbutton2") == 0)   return MouseButton::X2;
        if (strcmp(s, "x1") == 0)         return MouseButton::X1;
        if (strcmp(s, "x2") == 0)         return MouseButton::X2;
        return MouseButton::LEFT;
    }
    long val = PyLong_AsLong(obj);
    switch (val) {
        case 1: return MouseButton::LEFT;
        case 2: return MouseButton::RIGHT;
        case 3: return MouseButton::MIDDLE;
        case 4: return MouseButton::X1;
        case 5: return MouseButton::X2;
        default: return MouseButton::LEFT;
    }
}

// ==================== Helper: PyObject -> KeyCode ====================

static KeyCode key_name_to_vk(const char* s) {
    if (strcmp(s, "space") == 0)     return KeyCode::SPACE;
    if (strcmp(s, "shift") == 0)     return KeyCode::SHIFT;
    if (strcmp(s, "lshift") == 0)    return KeyCode::LSHIFT;
    if (strcmp(s, "rshift") == 0)    return KeyCode::RSHIFT;
    if (strcmp(s, "ctrl") == 0)      return KeyCode::CTRL;
    if (strcmp(s, "lctrl") == 0)     return KeyCode::LCTRL;
    if (strcmp(s, "rctrl") == 0)     return KeyCode::RCTRL;
    if (strcmp(s, "alt") == 0)       return KeyCode::ALT;
    if (strcmp(s, "lalt") == 0)      return KeyCode::LALT;
    if (strcmp(s, "ralt") == 0)      return KeyCode::RALT;
    if (strcmp(s, "win") == 0)       return KeyCode::LWIN;
    if (strcmp(s, "enter") == 0)     return KeyCode::ENTER;
    if (strcmp(s, "escape") == 0)    return KeyCode::ESCAPE;
    if (strcmp(s, "tab") == 0)       return KeyCode::TAB;
    if (strcmp(s, "back") == 0)      return KeyCode::BACK;
    if (strcmp(s, "delete") == 0)    return KeyCode::DEL;
    if (strcmp(s, "insert") == 0)    return KeyCode::INSERT;
    if (strcmp(s, "home") == 0)      return KeyCode::HOME;
    if (strcmp(s, "end") == 0)       return KeyCode::END;
    if (strcmp(s, "pageup") == 0)    return KeyCode::PRIOR;
    if (strcmp(s, "pagedown") == 0)  return KeyCode::NEXT;
    if (strcmp(s, "up") == 0)        return KeyCode::UP;
    if (strcmp(s, "down") == 0)      return KeyCode::DOWN;
    if (strcmp(s, "left") == 0)      return KeyCode::LEFT;
    if (strcmp(s, "right") == 0)     return KeyCode::RIGHT;
    if (strcmp(s, "capslock") == 0)  return KeyCode::CAPS_LOCK;
    if (strcmp(s, "numlock") == 0)   return KeyCode::NUM_LOCK;
    if (strcmp(s, "scrolllock") == 0) return KeyCode::SCROLL_LOCK;
    if (strcmp(s, "f1") == 0)        return KeyCode::F1;
    if (strcmp(s, "f2") == 0)        return KeyCode::F2;
    if (strcmp(s, "f3") == 0)        return KeyCode::F3;
    if (strcmp(s, "f4") == 0)        return KeyCode::F4;
    if (strcmp(s, "f5") == 0)        return KeyCode::F5;
    if (strcmp(s, "f6") == 0)        return KeyCode::F6;
    if (strcmp(s, "f7") == 0)        return KeyCode::F7;
    if (strcmp(s, "f8") == 0)        return KeyCode::F8;
    if (strcmp(s, "f9") == 0)        return KeyCode::F9;
    if (strcmp(s, "f10") == 0)       return KeyCode::F10;
    if (strcmp(s, "f11") == 0)       return KeyCode::F11;
    if (strcmp(s, "f12") == 0)       return KeyCode::F12;
    return static_cast<KeyCode>(0);
}

// ASCII → USB HID Keyboard Usage ID (single character fallback)
static uint16_t ascii_to_usb(char c) {
    if (c >= 'A' && c <= 'Z') return 0x04 + (c - 'A');
    if (c >= 'a' && c <= 'z') return 0x04 + (c - 'a');
    if (c >= '1' && c <= '9') return 0x1E + (c - '1');
    if (c == '0') return 0x27;
    switch (c) {
        case ' ':  return 0x2C;
        case '-':  return 0x2D;
        case '=':  return 0x2E;
        case '[':  return 0x2F;
        case ']':  return 0x30;
        case '\\': return 0x31;
        case ';':  return 0x33;
        case '\'': return 0x34;
        case '`':  return 0x35;
        case ',':  return 0x36;
        case '.':  return 0x37;
        case '/':  return 0x38;
        case '\n': return 0x28;
        case '\t': return 0x2B;
        default:   return 0x00;
    }
}

static KeyCode py_to_vk(PyObject* vk_obj) {
    if (PyUnicode_Check(vk_obj)) {
        Py_ssize_t len;
        const char* s = PyUnicode_AsUTF8AndSize(vk_obj, &len);
        if (!s) return KeyCode::ENTER;
        KeyCode code = key_name_to_vk(s);
        if (static_cast<uint16_t>(code) != 0) return code;
        // Single character fallback: ASCII → USB HID Usage ID
        if (len == 1) return static_cast<KeyCode>(ascii_to_usb(s[0]));
        return KeyCode::ENTER;
    }
    long val = PyLong_AsLong(vk_obj);
    return static_cast<KeyCode>((uint16_t)val);
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

    MouseButton btn = py_to_mouse_button(btn_obj);
    bool result;

    switch (op) {
        case 0: result = mouse_down(btn); break;
        case 1: result = mouse_up(btn);   break;
        default: result = mouse_click(btn); break;
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

    KeyCode vk = py_to_vk(vk_obj);
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

    std::vector<KeyCode> codes;
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

// ==================== Enum value maps (single source of truth = C++ header) ====================

static PyObject* hid_get_mouse_button_map(PyObject* self, PyObject* args) {
    PyObject* d = PyDict_New();
    PyDict_SetItemString(d, "LEFT",   PyLong_FromLong((long)MouseButton::LEFT));
    PyDict_SetItemString(d, "RIGHT",  PyLong_FromLong((long)MouseButton::RIGHT));
    PyDict_SetItemString(d, "MIDDLE", PyLong_FromLong((long)MouseButton::MIDDLE));
    PyDict_SetItemString(d, "X1",     PyLong_FromLong((long)MouseButton::X1));
    PyDict_SetItemString(d, "X2",     PyLong_FromLong((long)MouseButton::X2));
    return d;
}

#define KEY_CODE_ENTRY(name) PyDict_SetItemString(d, #name, PyLong_FromLong((long)KeyCode::name))

static PyObject* hid_get_key_code_map(PyObject* self, PyObject* args) {
    PyObject* d = PyDict_New();

    KEY_CODE_ENTRY(LBUTTON);   KEY_CODE_ENTRY(RBUTTON);
    KEY_CODE_ENTRY(CANCEL);    KEY_CODE_ENTRY(MBUTTON);
    KEY_CODE_ENTRY(BACK);      KEY_CODE_ENTRY(TAB);
    KEY_CODE_ENTRY(CLEAR);     KEY_CODE_ENTRY(ENTER);
    KEY_CODE_ENTRY(SHIFT);     KEY_CODE_ENTRY(CTRL);
    KEY_CODE_ENTRY(ALT);       KEY_CODE_ENTRY(PAUSE);
    KEY_CODE_ENTRY(CAPS_LOCK); KEY_CODE_ENTRY(ESCAPE);
    KEY_CODE_ENTRY(SPACE);     KEY_CODE_ENTRY(PRIOR);
    KEY_CODE_ENTRY(NEXT);      KEY_CODE_ENTRY(END);
    KEY_CODE_ENTRY(HOME);      KEY_CODE_ENTRY(LEFT);
    KEY_CODE_ENTRY(UP);        KEY_CODE_ENTRY(RIGHT);
    KEY_CODE_ENTRY(DOWN);      KEY_CODE_ENTRY(SELECT);
    KEY_CODE_ENTRY(PRINT);     KEY_CODE_ENTRY(SNAPSHOT);
    KEY_CODE_ENTRY(INSERT);    KEY_CODE_ENTRY(DEL);
    KEY_CODE_ENTRY(NUM0);      KEY_CODE_ENTRY(NUM1);
    KEY_CODE_ENTRY(NUM2);      KEY_CODE_ENTRY(NUM3);
    KEY_CODE_ENTRY(NUM4);      KEY_CODE_ENTRY(NUM5);
    KEY_CODE_ENTRY(NUM6);      KEY_CODE_ENTRY(NUM7);
    KEY_CODE_ENTRY(NUM8);      KEY_CODE_ENTRY(NUM9);
    KEY_CODE_ENTRY(A);         KEY_CODE_ENTRY(B);
    KEY_CODE_ENTRY(C);         KEY_CODE_ENTRY(D);
    KEY_CODE_ENTRY(E);         KEY_CODE_ENTRY(F);
    KEY_CODE_ENTRY(G);         KEY_CODE_ENTRY(H);
    KEY_CODE_ENTRY(I);         KEY_CODE_ENTRY(J);
    KEY_CODE_ENTRY(K);         KEY_CODE_ENTRY(L);
    KEY_CODE_ENTRY(M);         KEY_CODE_ENTRY(N);
    KEY_CODE_ENTRY(O);         KEY_CODE_ENTRY(P);
    KEY_CODE_ENTRY(Q);         KEY_CODE_ENTRY(R);
    KEY_CODE_ENTRY(S);         KEY_CODE_ENTRY(T);
    KEY_CODE_ENTRY(U);         KEY_CODE_ENTRY(V);
    KEY_CODE_ENTRY(W);         KEY_CODE_ENTRY(X);
    KEY_CODE_ENTRY(Y);         KEY_CODE_ENTRY(Z);
    KEY_CODE_ENTRY(LWIN);      KEY_CODE_ENTRY(RWIN);
    KEY_CODE_ENTRY(APPS);      KEY_CODE_ENTRY(NUMPAD0);
    KEY_CODE_ENTRY(NUMPAD1);   KEY_CODE_ENTRY(NUMPAD2);
    KEY_CODE_ENTRY(NUMPAD3);   KEY_CODE_ENTRY(NUMPAD4);
    KEY_CODE_ENTRY(NUMPAD5);   KEY_CODE_ENTRY(NUMPAD6);
    KEY_CODE_ENTRY(NUMPAD7);   KEY_CODE_ENTRY(NUMPAD8);
    KEY_CODE_ENTRY(NUMPAD9);   KEY_CODE_ENTRY(MULTIPLY);
    KEY_CODE_ENTRY(ADD);       KEY_CODE_ENTRY(SEPARATOR);
    KEY_CODE_ENTRY(SUBTRACT);  KEY_CODE_ENTRY(DECIMAL);
    KEY_CODE_ENTRY(DIVIDE);    KEY_CODE_ENTRY(F1);
    KEY_CODE_ENTRY(F2);        KEY_CODE_ENTRY(F3);
    KEY_CODE_ENTRY(F4);        KEY_CODE_ENTRY(F5);
    KEY_CODE_ENTRY(F6);        KEY_CODE_ENTRY(F7);
    KEY_CODE_ENTRY(F8);        KEY_CODE_ENTRY(F9);
    KEY_CODE_ENTRY(F10);       KEY_CODE_ENTRY(F11);
    KEY_CODE_ENTRY(F12);       KEY_CODE_ENTRY(F13);
    KEY_CODE_ENTRY(F14);       KEY_CODE_ENTRY(F15);
    KEY_CODE_ENTRY(F16);       KEY_CODE_ENTRY(F17);
    KEY_CODE_ENTRY(F18);       KEY_CODE_ENTRY(F19);
    KEY_CODE_ENTRY(F20);       KEY_CODE_ENTRY(F21);
    KEY_CODE_ENTRY(F22);       KEY_CODE_ENTRY(F23);
    KEY_CODE_ENTRY(F24);       KEY_CODE_ENTRY(NUM_LOCK);
    KEY_CODE_ENTRY(SCROLL_LOCK); KEY_CODE_ENTRY(LSHIFT);
    KEY_CODE_ENTRY(RSHIFT);    KEY_CODE_ENTRY(LCTRL);
    KEY_CODE_ENTRY(RCTRL);     KEY_CODE_ENTRY(LALT);
    KEY_CODE_ENTRY(RALT);      KEY_CODE_ENTRY(BROWSER_BACK);
    KEY_CODE_ENTRY(BROWSER_FORWARD);  KEY_CODE_ENTRY(BROWSER_REFRESH);
    KEY_CODE_ENTRY(BROWSER_STOP);     KEY_CODE_ENTRY(BROWSER_SEARCH);
    KEY_CODE_ENTRY(BROWSER_FAVORITES); KEY_CODE_ENTRY(BROWSER_HOME);
    KEY_CODE_ENTRY(VOLUME_MUTE);  KEY_CODE_ENTRY(VOLUME_DOWN);
    KEY_CODE_ENTRY(VOLUME_UP);    KEY_CODE_ENTRY(MEDIA_NEXT_TRACK);
    KEY_CODE_ENTRY(MEDIA_PREV_TRACK); KEY_CODE_ENTRY(MEDIA_STOP);
    KEY_CODE_ENTRY(MEDIA_PLAY_PAUSE); KEY_CODE_ENTRY(LAUNCH_MAIL);
    KEY_CODE_ENTRY(LAUNCH_MEDIA_SELECT); KEY_CODE_ENTRY(LAUNCH_APP1);
    KEY_CODE_ENTRY(LAUNCH_APP2); KEY_CODE_ENTRY(OEM_1);
    KEY_CODE_ENTRY(OEM_PLUS);   KEY_CODE_ENTRY(OEM_COMMA);
    KEY_CODE_ENTRY(OEM_MINUS);  KEY_CODE_ENTRY(OEM_PERIOD);
    KEY_CODE_ENTRY(OEM_2);      KEY_CODE_ENTRY(OEM_3);
    KEY_CODE_ENTRY(OEM_4);      KEY_CODE_ENTRY(OEM_5);
    KEY_CODE_ENTRY(OEM_6);      KEY_CODE_ENTRY(OEM_7);
    KEY_CODE_ENTRY(OEM_8);      KEY_CODE_ENTRY(OEM_102);
    KEY_CODE_ENTRY(ATTN);       KEY_CODE_ENTRY(CRSEL);
    KEY_CODE_ENTRY(EXSEL);      KEY_CODE_ENTRY(EREOF);
    KEY_CODE_ENTRY(PLAY);       KEY_CODE_ENTRY(PA1);
    KEY_CODE_ENTRY(OEM_CLEAR);

    return d;
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
    {"get_mouse_button_map",   (PyCFunction)hid_get_mouse_button_map,   METH_NOARGS,
     "Return dict of MouseButton enum names to values."},
    {"get_key_code_map",       (PyCFunction)hid_get_key_code_map,       METH_NOARGS,
     "Return dict of KeyCode enum names to values."},
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
