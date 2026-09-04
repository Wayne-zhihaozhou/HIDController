// raw_input_bindings.cpp — Python C API bindings for HIDController raw_input
// HIDController project — Windows RAW INPUT keyboard and mouse event tracker
#include <Python.h>
#include "../include/hid_controller.h"

// ==================== Module state ====================

struct RawInputState {
    // Python callback objects (ref counted)
    PyObject* mouse_cb = nullptr;      // callable(device_handle, dx, dy)
    PyObject* key_cb = nullptr;        // callable(device_handle, vkey, is_down)
    PyObject* mouse_button_cb = nullptr; // callable(device_handle, button, is_down)
    PyObject* wheel_cb = nullptr;      // callable(device_handle, wheel_delta, horizontal)
};

static RawInputState g_state = {};

// ==================== C callback wrappers ====================

#define PYTHON_CB_INVOKE(cb_field, fmt, ...) \
    do { \
        if (!g_state.cb_field || g_state.cb_field == Py_None) return; \
        PyGILState_STATE _gstate = PyGILState_Ensure(); \
        PyObject* _args = Py_BuildValue(fmt, __VA_ARGS__); \
        if (_args) { \
            PyObject* _result = PyObject_CallObject(g_state.cb_field, _args); \
            Py_XDECREF(_result); \
        } \
        Py_XDECREF(_args); \
        PyGILState_Release(_gstate); \
    } while(0)

static void mouse_callback_wrapper(uintptr_t device_handle, int32_t dx, int32_t dy) {
    PYTHON_CB_INVOKE(mouse_cb, "(Kll)", device_handle, (long)dx, (long)dy);
}

static void key_callback_wrapper(uintptr_t device_handle, uint16_t vkey, bool is_down) {
    PYTHON_CB_INVOKE(key_cb, "(KiO)", device_handle, vkey, is_down ? Py_True : Py_False);
}

static void mouse_button_callback_wrapper(uintptr_t device_handle, uint32_t button, bool is_down) {
    PYTHON_CB_INVOKE(mouse_button_cb, "(KKO)", device_handle, (unsigned long long)button, is_down ? Py_True : Py_False);
}

static void wheel_callback_wrapper(uintptr_t device_handle, int32_t wheel_delta, int32_t horizontal) {
    PYTHON_CB_INVOKE(wheel_cb, "(KiO)", device_handle, wheel_delta, horizontal ? Py_True : Py_False);
}

// ==================== Module functions ====================

static PyObject* raw_start_input_tracking(PyObject* self, PyObject* args) {
    PyObject* mouse_cb = nullptr;
    PyObject* key_cb = nullptr;
    PyObject* mouse_button_cb = nullptr;
    PyObject* wheel_cb = nullptr;

    // All 4 callbacks are required — matches C++ API exactly
    if (!PyArg_ParseTuple(args, "OOOO", &mouse_cb, &key_cb, &mouse_button_cb, &wheel_cb))
        return NULL;

    if (!PyCallable_Check(mouse_cb)) {
        PyErr_SetString(PyExc_TypeError, "mouse_callback must be callable");
        return NULL;
    }
    if (!PyCallable_Check(key_cb)) {
        PyErr_SetString(PyExc_TypeError, "key_callback must be callable");
        return NULL;
    }
    if (!PyCallable_Check(mouse_button_cb)) {
        PyErr_SetString(PyExc_TypeError, "mouse_button_callback must be callable");
        return NULL;
    }
    if (!PyCallable_Check(wheel_cb)) {
        PyErr_SetString(PyExc_TypeError, "wheel_callback must be callable");
        return NULL;
    }

    // Release old refs, steal new refs (caller no longer needs them)
    Py_XSETREF(g_state.mouse_cb, Py_XNewRef(mouse_cb));
    Py_XSETREF(g_state.key_cb, Py_XNewRef(key_cb));
    Py_XSETREF(g_state.mouse_button_cb, Py_XNewRef(mouse_button_cb));
    Py_XSETREF(g_state.wheel_cb, Py_XNewRef(wheel_cb));

    bool started = start_input_tracking(
        mouse_callback_wrapper,
        key_callback_wrapper,
        mouse_button_callback_wrapper,
        wheel_callback_wrapper);

    return PyBool_FromLong((long)started);
}

static PyObject* raw_stop_input_tracking(PyObject* self, PyObject* args) {
    // Release Python callbacks after stopping
    stop_input_tracking();
    Py_XSETREF(g_state.mouse_cb, nullptr);
    Py_XSETREF(g_state.key_cb, nullptr);
    Py_XSETREF(g_state.mouse_button_cb, nullptr);
    Py_XSETREF(g_state.wheel_cb, nullptr);
    Py_RETURN_NONE;
}

static PyObject* raw_is_tracking(PyObject* self, PyObject* args) {
    (void)args;
    return PyBool_FromLong((long)is_tracking());
}

static PyObject* raw_get_mouse_delta(PyObject* self, PyObject* args) {
    unsigned long long device_handle = 0;
    if (!PyArg_ParseTuple(args, "|K", &device_handle))
        return NULL;

    long dx, dy;
    if (PyTuple_GET_SIZE(args) > 0) {
        auto d = get_mouse_delta(static_cast<uintptr_t>(device_handle));
        dx = d.first;
        dy = d.second;
    } else {
        auto d = get_mouse_delta();
        dx = d.first;
        dy = d.second;
    }
    return Py_BuildValue("(ll)", dx, dy);
}

static PyObject* raw_get_pressed_keys(PyObject* self, PyObject* args) {
    (void)args;
    std::vector<uint16_t> keys = get_pressed_keys();

    PyObject* list = PyList_New((Py_ssize_t)keys.size());
    for (size_t i = 0; i < keys.size(); i++) {
        PyList_SET_ITEM(list, (Py_ssize_t)i, PyLong_FromUnsignedLong((unsigned long)keys[i]));
    }
    return list;
}

static PyObject* raw_register_raw_input(PyObject* self, PyObject* args) {
    uintptr_t hwnd = 0;
    if (!PyArg_ParseTuple(args, "K", &hwnd))
        return NULL;

    bool result = register_raw_input(reinterpret_cast<HWND>(hwnd));
    return PyBool_FromLong((long)result);
}

static PyObject* raw_get_device_name(PyObject* self, PyObject* args) {
    uintptr_t device_handle = 0;
    if (!PyArg_ParseTuple(args, "K", &device_handle))
        return NULL;

    // Use the public C API (not the internal _impl)
    wchar_t name[512];
    uint32_t name_length = 512;
    bool result = get_device_name(device_handle, name, &name_length);
    if (!result) {
        // Buffer too small — retry with dynamic buffer
        if (name_length > 0 && name_length < 1024) {
            std::vector<wchar_t> buf(name_length + 1, L'\0');
            name_length = static_cast<uint32_t>(buf.size());
            if (get_device_name(device_handle, buf.data(), &name_length)) {
                return PyUnicode_FromWideChar(buf.data(), static_cast<Py_ssize_t>(name_length));
            }
        }
        PyErr_SetString(PyExc_RuntimeError, "get_device_name failed");
        return NULL;
    }
    return PyUnicode_FromWideChar(name, static_cast<Py_ssize_t>(name_length));
}

// ==================== Method table ====================

static PyMethodDef RawInputMethods[] = {
    {"start_input_tracking", (PyCFunction)raw_start_input_tracking, METH_VARARGS,
     "Start tracking keyboard and mouse events via RAW INPUT.\n"
     "Args: mouse_callback, key_callback, mouse_button_callback, wheel_callback (all required callables)\n"
     "wheel_callback(device_handle, wheel_delta, horizontal) — horizontal=1 for hwheel, 0 for vertical"},
    {"stop_input_tracking", (PyCFunction)raw_stop_input_tracking, METH_NOARGS,
     "Stop tracking"},
    {"is_tracking", (PyCFunction)raw_is_tracking, METH_NOARGS,
     "Check if tracking is running"},
    {"get_mouse_delta", (PyCFunction)raw_get_mouse_delta, METH_VARARGS,
     "Get accumulated mouse delta since last call, as (dx, dy).\n"
     "Args: device_handle (int, optional) — omit to sum across all devices"},
    {"get_pressed_keys", (PyCFunction)raw_get_pressed_keys, METH_NOARGS,
     "Get list of currently pressed virtual key codes"},
    {"register_raw_input", (PyCFunction)raw_register_raw_input, METH_VARARGS,
     "Register a window to receive raw input events"},
    {"get_device_name", (PyCFunction)raw_get_device_name, METH_VARARGS,
     "Get the device name for a raw input device handle"},
    {NULL, NULL, 0, NULL}
};

// ==================== Module definition ====================

static struct PyModuleDef raw_input_module = {
    PyModuleDef_HEAD_INIT,
    "raw_input",
    "HIDController - Windows RAW INPUT keyboard and mouse event tracker",
    -1,
    RawInputMethods
};

PyMODINIT_FUNC PyInit_raw_input(void) {
    return PyModule_Create(&raw_input_module);
}
