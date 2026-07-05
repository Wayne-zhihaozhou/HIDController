# Build

编译 HIDController 项目。

## 工具路径

```bash
CMAKE="C:/Program Files/Microsoft Visual Studio/2022/Professional/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
MSBUILD="C:/Program Files/Microsoft Visual Studio/2022/Professional/MSBuild/Current/Bin/MSBuild.exe"
```

## 编译流程

### 1. Build hid-lib

```bash
cd hid-lib
"$MSBUILD" hid_controller.sln -p:Configuration=Release -p:Platform=x64 -m
```

### 2. Build Python bindings

```bash
cd hid-py
rm -rf build CMakeFiles CMakeCache.txt
"$CMAKE" -B build -G "Visual Studio 17 2022" -T host=x64
"$MSBUILD" build/hid_controller_python.sln -p:Configuration=Release -p:Platform=x64 -m
```

## 验证

```bash
cd hid-py
PYTHONPATH=./dist python -c "import hid_send; import raw_input; print('OK')"
```

## 注意事项

- 仅 Release|x64
- CMake 每次配置前需清理旧 build 目录
