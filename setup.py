import sys

import pybind11
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

# ==================== HIDController 主扩展模块（输出：控制键盘鼠标） ====================
_extension_src_files = [
    "src/bindings/main_bindings.cpp",
    "src/core/IbSendMouse.cpp",
    "src/core/IbSendKeyboard.cpp",
    "src/core/Logitech.cpp",
    "src/core/LogitechDriver.cpp",
    "src/core/pch.cpp",
]

# ==================== input_tracker 扩展模块（输入：检测键盘鼠标事件） ====================
input_tracker_src_files = [
    "src/bindings/input_tracker_bindings.cpp",
]

# 创建扩展模块
ext_modules = [
    # 主模块：通过 Logitech HID 报告控制键盘鼠标
    Pybind11Extension(
        "hid_controller._extension",
        _extension_src_files,
        include_dirs=[
            str(pybind11.get_include()),
            "include",
        ],
        cxx_std=17,
        define_macros=[("DLL1_EXPORTS", "1")],
    ),
    # 输入追踪模块：通过 RAW INPUT API 检测键盘鼠标事件
    Pybind11Extension(
        "hid_controller.input_tracker",
        input_tracker_src_files,
        include_dirs=[
            str(pybind11.get_include()),
            "include",
            "src/core",
        ],
        cxx_std=17,
    ),
]

# Windows 需要链接的库
if sys.platform == "win32":
    # 主模块：需要 user32, kernel32, advapi32, winmm
    ext_modules[0].libraries.extend(["user32", "kernel32", "advapi32", "winmm"])
    # input_tracker 模块：需要 user32 (RAW INPUT API)
    ext_modules[1].libraries.extend(["user32", "kernel32"])


setup(
    name="hid-controller",
    version="1.0.0",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.8",
    packages=["hid_controller"],
)
