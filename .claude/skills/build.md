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

产物:

| 产物 | 路径 |
| --- | --- |
| 核心 DLL | `hid-lib/build/Release/hid_controller.dll` |
| 导入库 | `hid-lib/build/Release/hid_controller.lib` |
| RAW INPUT 测试 | `hid-lib/test_raw_input/build/Release/test_raw_input.exe` |
| 鼠标轨迹测试 | `hid-lib/test_mouse_wave/build/Release/test_mouse_wave.exe` |
| 输出 API 测试 | `hid-lib/test_hid_controller/build/Release/test_hid_controller.exe` |

## 注意事项

| 事项 | 说明 |
| --- | --- |
| 配置 | 仅 Release\|x64，从不构建 Debug |
| `-m` 并行 | 可安全使用。三个测试工程都有指向 `hid_controller.vcxproj` 的 `<ProjectReference>`，MSBuild 据此推导构建顺序 |
| 运行测试可执行文件 | `test_hid_controller.exe` 启动即真实移动鼠标、点击、按键，会劫持当前桌面，不要在自动化流程里运行 |
