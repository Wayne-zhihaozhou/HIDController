from pybind11.setup_helpers import Pybind11Extension, build_ext
import pybind11

# 包含所有源文件：binding.cpp + 原有C++实现
src_files = [
    "binding.cpp",
    "src/IbSendMouse.cpp",
    "src/IbSendKeyboard.cpp",
    "src/Logitech.cpp",
    "src/LogitechDriver.cpp",
    "src/pch.cpp",
]

# 创建扩展模块
ext_modules = [
    Pybind11Extension(
        "hid_controller._extension",
        src_files,
        include_dirs=[
            str(pybind11.get_include()),
            "include",
        ],
        cxx_std=17,
        define_macros=[("DLL1_EXPORTS", "1")],
    ),
]

# Windows 需要链接的库
import sys
if sys.platform == "win32":
    ext_modules[0].libraries.extend(["user32", "kernel32", "advapi32", "winmm"])

from setuptools import setup

setup(
    name="hid-controller",
    version="1.0.0",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.8",
    packages=["hid_controller"],
)
