from pybind11.setup_helpers import Pybind11Extension, build_ext
from pybind11 import get_cmake_dir
import pybind11
import os

# 只包含binding.cpp，不包含原始项目源文件，因为我们直接使用Windows API
src_files = [
    "binding.cpp"
]

# 创建扩展模块
ext_modules = [
    Pybind11Extension(
        "my_input_lib",
        src_files,  # 只包含binding.cpp
        include_dirs=[
            "include",
            "dll"
        ],
        cxx_std=17,
        define_macros=[("DLL1_EXPORTS", "1")],  # 添加DLL导出定义
        libraries=["user32", "kernel32", "advapi32", "winmm"],  # 添加Windows API库，移除DLL库
    ),
]

from setuptools import setup

setup(
    name="my_input_lib",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.12",
)
