"""setup.py for HIDController Python package."""
import os
import shutil
import subprocess
from pathlib import Path

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name: str, sourcedir: str = "") -> None:
        super().__init__(name, sources=[])
        self.sourcedir = os.fspath(Path(sourcedir).resolve())


class CMakeBuild(build_ext):
    def build_extension(self, ext: CMakeExtension) -> None:
        ext_fullpath = Path(self.get_ext_fullpath(ext.name))
        extdir = ext_fullpath.parent.resolve()

        cmake_path = shutil.which("cmake")
        if not cmake_path:
            raise RuntimeError("cmake not found")

        build_dir = Path(self.build_lib) / "hid_controller" / "build" / "cmake"
        build_dir.mkdir(parents=True, exist_ok=True)

        config = "Release" if self.debug else "Debug"

        subprocess.run(
            [
                cmake_path,
                str(Path(ext.sourcedir).resolve()),
                "-G", "Visual Studio 17 2022",
                "-A", "x64",
                f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}",
                f"-DCMAKE_BUILD_TYPE={config}",
            ],
            cwd=str(build_dir),
            check=True,
        )

        subprocess.run(
            [cmake_path, "--build", ".", "--config", config, "--parallel"],
            cwd=str(build_dir),
            check=True,
        )


setup(
    ext_modules=[
        CMakeExtension("hid_controller.hid_send"),
        CMakeExtension("hid_controller.raw_input"),
    ],
    cmdclass={"build_ext": CMakeBuild},
)
