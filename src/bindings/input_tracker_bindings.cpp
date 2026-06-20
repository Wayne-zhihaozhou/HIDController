// input_tracker bindings - pybind11 module definition
// Includes the C++ implementation from input_tracker.cpp

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <windows.h>

namespace py = pybind11;

#include "../core/input_tracker.cpp"

PYBIND11_MODULE(input_tracker, m) {
    m.doc() = "HIDController - Windows RAW INPUT keyboard and mouse event tracker";

    m.def("start", &start_tracking,
          py::arg("mouse_callback") = py::none(),
          py::arg("key_callback") = py::none(),
          py::arg("mouse_button_callback") = py::none(),
          "Start tracking keyboard and mouse events via RAW INPUT");

    m.def("stop", &stop_tracking, "Stop tracking");

    m.def("is_tracking", &is_tracking, "Check if tracking is running");

    m.def("get_mouse_delta", &get_mouse_delta, "Get accumulated mouse delta since last call");
    m.def("get_pressed_keys", &get_pressed_keys, "Get list of currently pressed virtual key codes");
}
