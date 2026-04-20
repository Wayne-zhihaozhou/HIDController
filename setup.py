from pybind11.setup_helpers import Pybind11Extension, build_ext
import pybind11
import os
from shutil import copy
import glob

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
        "hid_controller",
        src_files,
        include_dirs=[
            "include",
        ],
        cxx_std=17,
        define_macros=[("DLL1_EXPORTS", "1")],
        libraries=["user32", "kernel32", "advapi32", "winmm"],
    ),
]

from setuptools import setup

class custom_build_ext(build_ext):
    """自定义构建命令，将 .pyd 文件输出到根目录"""
    def run(self):
        # 先执行标准构建
        super().run()

        # 将 .pyd 文件复制到根目录
        for ext in self.get_outputs():
            if ext.endswith(".pyd"):
                # 获取输出文件名
                filename = os.path.basename(ext)
                # 复制到根目录
                dest = os.path.join(os.getcwd(), filename)
                copy(ext, dest)
                print(f"Copying {ext} -> {dest}")

setup(
    name="hid_controller",
    ext_modules=ext_modules,
    cmdclass={"build_ext": custom_build_ext},
    zip_safe=False,
    python_requires=">=3.12",
)
