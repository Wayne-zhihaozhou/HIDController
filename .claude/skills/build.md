# Build

编译 HIDController 项目。

## 工具路径

```bash
MSBUILD="C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe"
```

## 编译流程

### Build hid-lib

```bash
cd hid-lib
"$MSBUILD" hid_controller.sln -p:Configuration=Release -p:Platform=x64 -m
```

产物: `hid-lib/build/Release/hid_controller.dll`

## 注意事项

- 仅 Release|x64
