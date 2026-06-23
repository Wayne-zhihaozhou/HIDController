// raw_input_bindings.cpp — Python C API bindings for HIDController raw_input
// HIDController project — Windows RAW INPUT keyboard and mouse event tracker
#include <Python.h>
#include <windows.h>
#include <stdint.h>
#include <vector>
#include <mutex>
#include <functional>
#include <atomic>
#include <thread>
#include "input_tracker_internal.h"
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

static void mouse_callback_wrapper(uintptr_t device_handle, int32_t dx, int32_t dy) {
    if (!g_state.mouse_cb || g_state.mouse_cb == Py_None) return;

    // Acquire GIL and call Python
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject* args = Py_BuildValue("(Kll)", device_handle, (long)dx, (long)dy);
    if (args && g_state.mouse_cb != Py_None) {
        PyObject* result = PyObject_CallObject(g_state.mouse_cb, args);
        Py_XDECREF(result);
    }
    Py_XDECREF(args);
    PyGILState_Release(gstate);
}

static void key_callback_wrapper(uintptr_t device_handle, uint16_t vkey, bool is_down) {
    if (!g_state.key_cb || g_state.key_cb == Py_None) return;

    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject* args = Py_BuildValue("(Kib)", device_handle, vkey, is_down);
    if (args && g_state.key_cb != Py_None) {
        PyObject* result = PyObject_CallObject(g_state.key_cb, args);
        Py_XDECREF(result);
    }
    Py_XDECREF(args);
    PyGILState_Release(gstate);
}

static void mouse_button_callback_wrapper(uintptr_t device_handle, uint32_t button, bool is_down) {
    if (!g_state.mouse_button_cb || g_state.mouse_button_cb == Py_None) return;

    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject* args = Py_BuildValue("(KKO)", device_handle, (unsigned long long)button, is_down ? Py_True : Py_False);
    if (args && g_state.mouse_button_cb != Py_None) {
        PyObject* result = PyObject_CallObject(g_state.mouse_button_cb, args);
        Py_XDECREF(result);
    }
    Py_XDECREF(args);
    PyGILState_Release(gstate);
}

static void wheel_callback_wrapper(uintptr_t device_handle, int32_t wheel_delta, int32_t horizontal) {
    if (!g_state.wheel_cb || g_state.wheel_cb == Py_None) return;

    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject* args = Py_BuildValue("(KiO)", device_handle, wheel_delta, horizontal ? Py_True : Py_False);
    if (args && g_state.wheel_cb != Py_None) {
        PyObject* result = PyObject_CallObject(g_state.wheel_cb, args);
        Py_XDECREF(result);
    }
    Py_XDECREF(args);
    PyGILState_Release(gstate);
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

    start_tracking_impl(
        mouse_callback_wrapper,
        key_callback_wrapper,
        mouse_button_callback_wrapper,
        wheel_callback_wrapper);

    // start_tracking_impl is void; C++ wrapper always returns true
    return PyBool_FromLong(true);
}

static PyObject* raw_stop_input_tracking(PyObject* self, PyObject* args) {
    // Release Python callbacks after stopping
    stop_tracking_impl();
    Py_XSETREF(g_state.mouse_cb, nullptr);
    Py_XSETREF(g_state.key_cb, nullptr);
    Py_XSETREF(g_state.mouse_button_cb, nullptr);
    Py_XSETREF(g_state.wheel_cb, nullptr);
    Py_RETURN_NONE;
}

static PyObject* raw_is_tracking(PyObject* self, PyObject* args) {
    (void)args;
    return PyBool_FromLong((long)is_tracking_impl());
}

static PyObject* raw_get_mouse_delta(PyObject* self, PyObject* args) {
    (void)args;
    auto [dx, dy] = get_mouse_delta();
    return Py_BuildValue("(l,l)", dx, dy);
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

    bool result = hid_controller_register_raw_input(reinterpret_cast<HWND>(hwnd));
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
    {"get_mouse_delta", (PyCFunction)raw_get_mouse_delta, METH_NOARGS,
     "Get accumulated mouse delta since last call"},
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
