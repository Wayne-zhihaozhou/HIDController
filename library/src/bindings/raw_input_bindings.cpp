// raw_input_bindings.cpp — pybind11 module definition
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <windows.h>
#include "input_tracker_internal.h"

namespace py = pybind11;

PYBIND11_MODULE(raw_input, m) {
    m.doc() = "HIDController - Windows RAW INPUT keyboard and mouse event tracker";

    m.def("start",
        [](py::object mouse_cb = py::none(),
           py::object key_cb = py::none(),
           py::object mouse_button_cb = py::none()) {
            mouse_move_fn mfn = nullptr;
            keyboard_fn kfn = nullptr;
            mouse_button_fn mbfn = nullptr;
            if (!mouse_cb.is_none())
                mfn = mouse_cb.cast<std::function<void(uintptr_t, long, long)>>();
            if (!key_cb.is_none())
                kfn = key_cb.cast<std::function<void(uintptr_t, uint16_t, bool)>>();
            if (!mouse_button_cb.is_none())
                mbfn = mouse_button_cb.cast<std::function<void(uintptr_t, uint32_t, bool)>>();
            start_tracking_impl(mfn, kfn, mbfn);
        },
        py::arg("mouse_callback") = py::none(),
        py::arg("key_callback") = py::none(),
        py::arg("mouse_button_callback") = py::none(),
        "Start tracking keyboard and mouse events via RAW INPUT");

    m.def("stop", &stop_tracking_impl, "Stop tracking");
    m.def("is_tracking", &is_tracking_impl, "Check if tracking is running");

    m.def("get_mouse_delta", []() -> py::tuple {
        long dx, dy;
        get_mouse_delta_impl(&dx, &dy);
        return py::make_tuple(dx, dy);
    }, "Get accumulated mouse delta since last call");

    m.def("get_pressed_keys", []() -> py::list {
        uint16_t keys[64];
        uint32_t count = 0;
        get_pressed_keys_impl(keys, &count, 64);
        py::list result;
        for (uint32_t i = 0; i < count; ++i)
            result.append(keys[i]);
        return result;
    }, "Get list of currently pressed virtual key codes");
}
