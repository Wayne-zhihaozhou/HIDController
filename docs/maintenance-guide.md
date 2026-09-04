# HIDController 维护指南

面向刚接手本项目的维护者。目标不是介绍功能，而是让你**在不弄坏现有东西的前提下做改动**。

本文只写别处没有记录的内容。以下三份文档不在本文重复，请直接跳转：

| 文档 | 覆盖内容 | 与本文的关系 |
|---|---|---|
| [api-reference.md](./api-reference.md) | 逐函数 API 说明、参数、返回值 | 本文不重复任何函数签名。但该文档**有若干处与代码不符**，见第 6 节 |
| [naming-conventions.md](./naming-conventions.md) | C++ / Python 命名规范 | 本文不重述命名规则，新增代码必须遵守该文档 |
| [../.claude/skills/build.md](../.claude/skills/build.md) | 一条 MSBuild 命令 | 本文**取代**它。该文件记录的命令带 `-m`，存在竞态，见第 6 节 |

---

## 1. 项目结构与职责

顶层布局与各自产出：

| 路径 | 职责 | 产出物 |
|---|---|---|
| `hid-lib/` | C++ 核心库工程根目录，`.sln` / `.vcxproj` / `.def` 均在此 | `hid-lib/build/Release/hid_controller.dll` + `.lib` + `.exp` + `.pdb` |
| `hid-lib/include/` | 公开头文件：`hid_controller.h`（唯一公开 API 头，含 `KeyCode` / `MouseButton` 枚举）、`logitech.h`、`logitech_driver.h`、`pch.h` | 无（头文件） |
| `hid-lib/src/core/` | 核心实现，7 个 `.cpp` 全部编入 DLL | 编入 `hid_controller.dll` |
| `hid-lib/src/bindings/` | Python 扩展模块源码，**不编入 DLL**，由 `hid-py/CMakeLists.txt` 单独编译 | 编成两个 `.pyd` |
| `hid-py/` | Python 打包层：`CMakeLists.txt`、`setup.py`、`pyproject.toml` | 无（构建脚本） |
| `hid-py/hid_controller/` | 可导入的 Python 包目录，`__init__.py` 是唯一手写 Python 文件 | `hid_send.cp312-win_amd64.pyd`、`raw_input.cp312-win_amd64.pyd`、随行 `hid_controller.dll` |
| `hid-py/temp/` | CMake 存放扩展模块自身 `.lib` / `.exp` 的垃圾桶，已 gitignore | 构建垃圾 |
| `docs/` | 全部文档，仅 3 个 md 文件 | 无 |
| `.claude/` | `settings.local.json` + `skills/build.md` | 无 |

`hid-lib/src/core/` 各文件分工：

| 文件 | 负责 |
|---|---|
| `logitech_driver.cpp` | 通过 `DeviceIoControl` 向罗技虚拟驱动下发 HID 报文，最底层 |
| `logitech.cpp` | `Logitech` 单例，维护按键 / 鼠标按钮状态并组装报文 |
| `ib_send_mouse.cpp` | 鼠标类公开 API 实现（移动、按键、滚轮、加速度、系数校准） |
| `ib_send_keyboard.cpp` | 键盘类公开 API 实现（`key_down` / `key_up` / `key_press` / `key_combo` / `key_seq` / `release_all_keys`） |
| `ib_key_intercept.cpp` | `WH_KEYBOARD_LL` 物理按键拦截 + 512 项环形队列 + 重放 |
| `input_tracker_impl.cpp` | RAW INPUT 后台窗口 + 消息循环 + 设备增量/按键集合，**文件级静态状态只允许存在于 DLL 内** |
| `hid_raw_input_dll.cpp` | 把 `input_tracker_impl` 的内部接口包装成公开 DLL 导出 |

三个测试工程，注意它们并不都在解决方案里：

| 测试工程 | 是否在 `hid_controller.sln` 中 | 说明 |
|---|---|---|
| `hid-lib/test_raw_input/` | 是 | RAW INPUT 监听测试 |
| `hid-lib/test_mouse_wave/` | 是 | 鼠标移动轨迹测试 |
| `hid-lib/test_hid_controller/` | **否** | 仅有 `.cpp` 和 `.vcxproj`，未被 `.sln` 引用，构建解决方案不会编译它 |

---

## 2. 双分支工作流（最重要的一节）

本项目使用两条长期分支，弄错会直接丢掉整个 Python 层。

| 分支 | 当前 commit（实测） | 用途 |
|---|---|---|
| `feature/cpp-only` | `19dd469` | 日常 C++ 开发。历史上**不含 Python 层** |
| `feature/python-bindings` | `19dd469` | 集成分支：合入 cpp-only 后重新同步 Python 绑定 |
| `master` | `e3d280f` | 提 PR 的基线分支 |

### 2.1 地雷的真实构造

历史上有**两个**独立的删除 commit，而不是一个。这一点很关键，因为恢复时要从两个不同的位置取文件：

| 删除 commit | 时间 | 删了什么 |
|---|---|---|
| `a44b3ae` "feat: remove Python bindings, switch to pure C++ project" | 2026-07-05 19:23 | `hid-py/CMakeLists.txt`、`hid-py/setup.py`、`hid-py/pyproject.toml`、`hid-py/hid_controller/__init__.py`，以及构建产物 `hid-py/hid_controller/hid_controller.dll`、`hid-py/temp/*.lib` / `*.exp`；同时改了 `.gitignore` |
| `54595ab` "1" | 2026-07-06 00:41 | `hid-lib/src/bindings/hid_send_bindings.cpp`、`hid-lib/src/bindings/raw_input_bindings.cpp` |

两个 commit 都已经是 `feature/cpp-only` 和 `feature/python-bindings` 的共同祖先。因此把 cpp-only 合进 python-bindings 时，这两次删除会被再次带入，Python 层整体消失。2026-09-04 就踩了这个坑。

### 2.2 关键陷阱：不能用 merge-base 恢复

先看 merge-base 是什么：

```bash
git merge-base feature/python-bindings feature/cpp-only
```

当前两条分支指向**同一个 commit**，所以 merge-base 就是 `19dd469`（即 HEAD 本身）。而 `19dd469` 里这 6 个文件**一个都没有**——它在两次删除之后。

| 恢复方式 | 结果 |
|---|---|
| `git checkout <merge-base> -- <paths>` | **失败**，merge-base 不含这些文件 |
| 从各自删除 commit 的父提交取文件 | 可行，见下 |

正确的恢复源：

| 要恢复的文件 | 取自 | 该 commit 的短号 |
|---|---|---|
| `hid-py/CMakeLists.txt`、`hid-py/setup.py`、`hid-py/pyproject.toml`、`hid-py/hid_controller/__init__.py` | `a44b3ae^` | `de44515` |
| `hid-lib/src/bindings/hid_send_bindings.cpp`、`hid-lib/src/bindings/raw_input_bindings.cpp` | `54595ab^` | `57fe145` |

### 2.3 恢复流程

第一步，正常合并（2026-09-04 那次是 fast-forward）：

```bash
git merge feature/cpp-only
```

第二步，恢复 4 个 `hid-py/` 文件：

```bash
git checkout a44b3ae^ -- hid-py/CMakeLists.txt hid-py/setup.py hid-py/pyproject.toml hid-py/hid_controller/__init__.py
```

第三步，恢复 2 个绑定源文件：

```bash
git checkout 54595ab^ -- hid-lib/src/bindings/hid_send_bindings.cpp hid-lib/src/bindings/raw_input_bindings.cpp
```

如果将来又出现新的删除 commit、短号记不住，可以用这条通用写法定位「最后一个含有该文件的 commit」，再取它的父提交：

```bash
git rev-list -n 1 --all -- hid-py/CMakeLists.txt
```

第四步，**绝对不要恢复**下列构建产物。它们在删除 commit 里也被一并删掉了，但那是好事：

| 不要恢复的路径 | 原因 |
|---|---|
| `hid-py/hid_controller/hid_controller.dll` | 构建产物。历史上入过库，导出表过期，会静默覆盖新构建的 DLL，见第 6 节 |
| `hid-py/temp/*.lib`、`hid-py/temp/*.exp` | 扩展模块自身的导入库，纯构建垃圾 |

第五步，按第 4 节的清单把绑定层与新的 C++ API 重新对齐，然后按第 5 节两阶段构建，最后跑第 7 节的验证清单。

---

## 3. Python 绑定层是怎么工作的

### 3.1 机制：手写 CPython C-API

这是新人最容易搞错的地方。实测两个绑定源文件都是：`#include <Python.h>`、`PyMethodDef` 方法表、`PyModuleDef` 模块定义、`PyMODINIT_FUNC PyInit_xxx`。

| 技术 | 本项目是否使用 |
|---|---|
| 手写 CPython C-API | **是**，唯一机制 |
| pybind11 | **否**。`input_tracker_impl.cpp` 首行注释明确写着 "no pybind11" |
| ctypes / cffi | **否**，没有任何 Python 侧的 DLL 动态加载代码 |

写绑定时请照抄现有两个文件的写法，不要引入 pybind11 或改成 ctypes。

### 3.2 两个扩展模块

| Python 模块 | 源文件 | `PyInit` 函数 | 覆盖的 API |
|---|---|---|---|
| `hid_controller.hid_send` | `hid-lib/src/bindings/hid_send_bindings.cpp` | `PyInit_hid_send` | 鼠标、键盘、按键拦截，外加两个枚举映射函数 |
| `hid_controller.raw_input` | `hid-lib/src/bindings/raw_input_bindings.cpp` | `PyInit_raw_input` | RAW INPUT 追踪与查询 |

### 3.3 为什么链接 `hid_controller.lib` 而不是 `GetProcAddress`

两个模块都通过 `target_link_libraries` 链接 `hid-lib/build/Release/hid_controller.lib`（导入库）。这是有意的架构选择：

| 导出符号 | 链接方式 | 返回类型 |
|---|---|---|
| 25 个 `extern "C"` 导出 | 导入库 | `bool` / `void` / `float` |
| `get_mouse_delta()`、`get_mouse_delta(uintptr_t)` | 导入库 | `std::pair<long,long>` **按值** |
| `get_pressed_keys()` | 导入库 | `std::vector<uint16_t>` **按值** |

后三个是 C++ mangled 名字、按值返回 STL 类型。链接导入库让编译器自己处理返回值布局与析构；`GetProcAddress` + 手写函数指针（更别说 ctypes）无法安全地跨 DLL 边界接收按值返回的 `std::pair` / `std::vector`。这就是本项目不用 ctypes 的根本原因。

`hid-py/CMakeLists.txt` 里还有一条容易被误改的注释：`raw_input` 模块**故意不编译** `input_tracker_impl.cpp`。因为追踪器的文件级静态状态必须只存在于 `hid_controller.dll` 内，否则模块会追踪进自己的私有副本，每次查询都读到 0。

### 3.4 为什么 DLL 必须复制到 `.pyd` 旁边

CPython 用 `LOAD_WITH_ALTERED_SEARCH_PATH` 加载扩展模块，因此导入时解析到的是**与 `.pyd` 同目录**的那个 DLL。`hid-py/CMakeLists.txt` 用 `POST_BUILD` 的 `copy_if_different` 把 `hid_controller.dll` 复制到 `$<TARGET_FILE_DIR:target>`。

| 后果 | 说明 |
|---|---|
| 不复制 | 导入即失败，找不到 DLL |
| 复制了过期的 DLL | 更糟：导入成功但导出表与 `.pyd` 不匹配，行为诡异或崩溃 |

### 3.5 `__init__.py` 的懒加载门面

`hid-py/hid_controller/__init__.py` 是一个基于 `__getattr__` 的懒加载门面，用**两份写死的名字列表**分派到两个扩展模块，另外提供 `start_tracking` / `stop_tracking` 两个别名。

枚举不在 Python 侧镜像，而是**运行时**构造：

| Python 名字 | 构造方式 |
|---|---|
| `MouseButton` | `IntEnum("MouseButton", _hid_send.get_mouse_button_map())` |
| `KeyCode` | `IntEnum("KeyCode", _hid_send.get_key_code_map())` |

这带来一条非常重要的不对称后果：

| 改动类型 | 是否需要改 Python |
|---|---|
| C++ 枚举**加一个值** | **不需要**。只要在 `hid_send_bindings.cpp` 的 `KEY_CODE_ENTRY` 宏列表里加一行，Python 侧自动出现 |
| C++ **加一个函数** | **需要**。必须编辑 `__getattr__` 的名字列表和 `__all__`，否则函数在包层面完全不可见 |

这不是理论风险，现在就有实例：`get_key_code_map` 和 `get_mouse_button_map` 在 `hid_send` 扩展模块里存在，但因为不在名字列表中，`hid_controller.get_key_code_map` 直接抛 `AttributeError`。要访问只能绕道 `import hid_controller.hid_send`。

### 3.6 `.def` 文件的角色

`hid-lib/hid_controller.def` 用**序号钉住**导出，当前 28 个序号（`@1` 到 `@28`），与头文件里 28 个 `DLLAPI` 声明一一对应。

| 序号 | 符号形态 | 说明 |
|---|---|---|
| `@1` – `@22` | 裸名字 | `extern "C"` 导出 |
| `@23` | `?get_mouse_delta@@YA?AU?$pair@JJ@std@@XZ` | C++ mangled，无参重载 |
| `@24` | `?get_mouse_delta@@YA?AU?$pair@JJ@std@@_K@Z` | C++ mangled，带 `device_handle` 重载 |
| `@25` | `?get_pressed_keys@@YA?AV?$vector@GV?$allocator@G@std@@@std@@XZ` | C++ mangled |
| `@26` – `@28` | 裸名字 | `begin_key_intercept` / `end_key_intercept` / `discard_queued_keys` |

即 3 个 mangled、25 个 `extern "C"`。改动 mangled 函数的签名会改变其修饰名，必须同步更新 `.def`，否则链接期报找不到符号。

---

## 4. 新增一个 C++ API 时必须同步改哪些地方

项目规则：**C++ 与 Python 的公开 API 必须双向完全一致**（见 [naming-conventions.md](./naming-conventions.md) 的「跨语言一致性」一节）。漏掉任何一步都会造成两边不同步。

| # | 改动位置 | 具体做什么 | 漏掉的后果 |
|---|---|---|---|
| 1 | `hid-lib/include/hid_controller.h` | 加声明。`extern "C"` 可返回的用 `DLLAPI`；返回 STL 类型的用 `DLLAPI_CPP` | 调用方看不到声明 |
| 2 | `hid-lib/src/core/<模块>.cpp` | 写实现。就近放到职责对应的文件，见第 1 节分工表 | 链接期未定义符号 |
| 3 | `hid-lib/hid_controller.vcxproj` | **仅当新建了 `.cpp` 文件时**，在 `<ItemGroup>` 里加一行 `<ClCompile Include="src\core\xxx.cpp" />` | 新文件根本不参与编译，症状是「代码明明写了却链接不到」 |
| 4 | `hid-lib/hid_controller.def` | 加导出并分配**新的下一个序号**（当前最大 `@28`，下一个是 `@29`）。返回 STL 类型的必须填完整 mangled 名 | 符号不导出，`.lib` 里没有，扩展模块链接失败 |
| 5 | `hid-lib/src/bindings/*_bindings.cpp` | 写包装函数：`PyArg_ParseTuple` 解参 → 调用 C++ → 转成 `PyObject*`。鼠标/键盘类放 `hid_send_bindings.cpp`，追踪类放 `raw_input_bindings.cpp` | Python 无法调用 |
| 6 | 同文件的 `PyMethodDef` 表 | 在 `HidSendMethods[]` 或 `RawInputMethods[]` 里加条目（含 `METH_VARARGS` / `METH_NOARGS` 和 docstring），必须加在结尾 `{NULL,...}` 哨兵**之前** | 包装函数存在但模块里查不到 |
| 7 | `hid-py/hid_controller/__init__.py` | 在 `__getattr__` 对应的名字元组里加名字，**并且**加进 `__all__` | 函数在包层面不可见（正是 3.5 节 `get_key_code_map` 的现状） |
| 8 | [api-reference.md](./api-reference.md) | 补上函数说明与返回值 | 文档与代码不一致，第 6 节列的那些坑就是这么攒出来的 |

新增**枚举值**是唯一的例外，只需改两处：`hid_controller.h` 的枚举体，以及 `hid_send_bindings.cpp` 里 `hid_get_key_code_map` 的 `KEY_CODE_ENTRY` 宏列表。Python 侧无需改动。

---

## 5. 构建流程

### 5.1 本机实测工具路径

| 工具 | 路径 | 实测结果 |
|---|---|---|
| MSBuild | `C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe` | 存在 |
| Python | `C:\Users\Admin\AppData\Local\Programs\Python\Python312\python.exe` | 存在，3.12.10 |
| CMake（PATH 上的） | `C:\Users\Admin\AppData\Local\Programs\Python\Python312\Scripts\cmake.exe` | 存在，4.3.4，**在 PATH 上** |
| CMake（VS 自带） | `C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe` | 存在，3.31.6-msvc6 |

关于 CMake 有一点需要留意：`hid-py/setup.py` 用 `shutil.which("cmake")` 找 CMake，因此走 `setup.py` 路线时实际用的是 PATH 上那个 pip 装的 4.3.4，而不是 VS 自带的 3.31.6。已有的 `hid-py/build/cmake-direct/CMakeCache.txt` 记录的却是 VS 自带那个，说明两者都被用过。生成器固定为 `Visual Studio 17 2022` + `-A x64`。

本机只有 VS 2022 一个安装（`C:\Program Files\Microsoft Visual Studio\` 下仅 `2022` 目录）。

### 5.2 两阶段构建，顺序不能颠倒

`hid-py/CMakeLists.txt` 会显式检查并在缺失时以 `FATAL_ERROR` 中止，报错信息是 `hid_controller.lib not found in ... — build hid-lib (Release|x64) first`。`.lib` 和 `.dll` 两者都检查。

阶段一，构建 hid-lib。**注意不要加 `-m`**，原因见第 6 节：

```bash
"C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" /c/Users/Admin/source/repos/HIDController/hid-lib/hid_controller.sln -p:Configuration=Release -p:Platform=x64
```

阶段二，构建两个 Python 扩展模块：

```bash
cd /c/Users/Admin/source/repos/HIDController/hid-py && "C:/Users/Admin/AppData/Local/Programs/Python/Python312/python.exe" setup.py build_ext --inplace
```

本项目**只构建 Release|x64**，从不构建 Debug。`.sln` 里虽然还留着 `Debug|x64` 配置项，但不要使用。

### 5.3 产物落点

| 产物 | 绝对路径 |
|---|---|
| 核心 DLL | `C:\Users\Admin\source\repos\HIDController\hid-lib\build\Release\hid_controller.dll` |
| 导入库 | `C:\Users\Admin\source\repos\HIDController\hid-lib\build\Release\hid_controller.lib` |
| `.exp` / `.pdb` | 同上目录 |
| `hid_send` 扩展 | `C:\Users\Admin\source\repos\HIDController\hid-py\hid_controller\hid_send.cp312-win_amd64.pyd` |
| `raw_input` 扩展 | `C:\Users\Admin\source\repos\HIDController\hid-py\hid_controller\raw_input.cp312-win_amd64.pyd` |
| 随行 DLL | `C:\Users\Admin\source\repos\HIDController\hid-py\hid_controller\hid_controller.dll` |
| 扩展模块自身 `.lib` / `.exp` | `C:\Users\Admin\source\repos\HIDController\hid-py\temp\` |
| 测试可执行文件 | `C:\Users\Admin\source\repos\HIDController\hid-lib\test_raw_input\build\Release\test_raw_input.exe` 等 |

---

## 6. 已知问题与陷阱

以下每一条都经过代码核实。

### 6.1 `.sln` 缺项目依赖，并行构建必败

实测结果：

| 检查项 | 结果 |
|---|---|
| `hid-lib/hid_controller.sln` 中 `ProjectSection(ProjectDependencies)` | **0 处**，完全没有 |
| 三个测试 `.vcxproj` 中 `<ProjectReference>` 指向 `hid_controller.vcxproj` | **0 处**，一个都没有 |
| 测试工程如何拿到库 | 靠 `<AdditionalDependencies>hid_controller.lib</AdditionalDependencies>` + `<AdditionalLibraryDirectories>` 硬写路径 |

因为没有依赖关系，MSBuild 加 `-m` 时会与 `hid_controller` 工程并行编译测试工程，`hid_controller.lib` 还没生成，链接器就报：

| 错误 | 含义 |
|---|---|
| `LNK1181: cannot open input file 'hid_controller.lib'` | 导入库尚未产出 |
| `MSB3073` | 上述链接失败导致的工程失败 |

顺序构建（不加 `-m`）总是成功。**`.claude/skills/build.md` 目前记录的命令带 `-m`，也就是说被文档化的构建命令本身是有竞态的。** 用第 5.2 节的命令替代。

顺带一个相关坑：`test_raw_input.vcxproj` 的 `AdditionalLibraryDirectories` 指向 `..\test_hid_controller\build\$(Configuration)\`，而 `test_hid_controller` 工程**不在 `.sln` 里**。也就是说该目录只有在有人单独构建过 `test_hid_controller` 之后才存在。

### 6.2 `api-reference.md` 与代码不一致

以下四处已逐条核实为**真实不一致**：

| 编号 | 文档的说法 | 代码实际情况 | 后果 |
|---|---|---|---|
| a | `start_input_tracking` 的任意回调都可传 `nullptr` | `hid-lib/src/core/hid_raw_input_dll.cpp` 把 mouse-move 和 wheel 两个回调包进**捕获式 lambda**，lambda 转成 `std::function` 后永远非空，内部 `if (mouse_callback_)` 判断恒真，于是无条件调用 | 这两个传 `nullptr` 会**崩溃**。keyboard 和 mouse-button 回调是裸函数指针直传，`std::function` 由空指针构造出来是空的，判断为假，因此**只有这两个**容忍 `nullptr` |
| b | `start_input_tracking` 成功返回 `true` | 函数体末尾是无条件 `return true;`；`start_tracking_impl` 返回 `void`，且若已在运行会直接 early-return | **没有失败信号**，返回值不可用于判断是否真的启动 |
| c | 「所有公开 DLL 函数返回 `bool`」 | 实测：9 个返回 `void`（`set_mouse_move_coefficient`、`auto_calibrate`、`disable_mouse_acceleration`、`enable_mouse_acceleration`、`release_all_keys`、`stop_input_tracking`、`begin_key_intercept`、`end_key_intercept`、`discard_queued_keys`），1 个返回 `float`（`get_mouse_move_coefficient`），3 个返回 STL 类型 | 错误处理一节的前提不成立 |
| d | `release_all_keys()` 释放所有按键 | `hid-lib/src/core/ib_send_keyboard.cpp` 实现里先 `logitech.release_all_keys()` **再 `logitech.release_all_mouse()`** | 它**同时释放鼠标按钮**，函数名有误导性 |

补充一点缓解措施：Python 侧的 `raw_start_input_tracking` 对 4 个回调逐个做 `PyCallable_Check`，传 `None` 会抛 `TypeError`。因此 (a) 只影响直接调 C++ 的代码，从 Python 调用不会踩到。

### 6.3 `begin_key_intercept` 会吞掉全部物理按键

`ib_key_intercept.cpp` 安装 `WH_KEYBOARD_LL` 钩子，`ll_keyboard_proc` 在拦截开启时对物理按键**返回 1**（即拦截，不下传），事件压入环形队列。

| 常量 | 值 |
|---|---|
| `kInterceptQueueSize`（拦截队列） | 512 |
| `kRecentSendCapacity`（识别自身程序发送的记录环） | 128 |

队列满之后新按键被静默丢弃，不会覆盖旧的。

| 收尾函数 | 行为 |
|---|---|
| `end_key_intercept()` | 关闭拦截，按入队顺序**重放**全部排队按键（`key_down` / `key_up`），重置队列，停止钩子 |
| `discard_queued_keys()` | 关闭拦截，**丢弃**全部排队按键不重放，停止钩子 |

**风险**：调用 `begin_key_intercept()` 后若因异常、提前 return 或测试崩溃而没走到收尾函数，钩子仍然装着、拦截仍然开着，用户会被锁死在自己的键盘外面。必须保证两个收尾函数之一一定执行（C++ 用 RAII 守卫，Python 用 `try` / `finally`）。**不要在自动化测试里随手调用它。**

顺便：`g_intercept_tail` 在第 134 行声明，在第 261 和 270 行被赋 0，但**从未被读取**，是一个死变量。改这个文件时不要以为它参与了环形缓冲逻辑——实际的读写游标只有 `g_intercept_head` 和 `g_intercept_count`。

### 6.4 构建产物绝不入库

历史上 `hid-py/hid_controller/hid_controller.dll` 是入过库的（`a44b3ae` 删除它时显示 58368 字节）。那个陈旧 DLL 的导出表已经过期，却因为就在 `.pyd` 旁边而被 `LOAD_WITH_ALTERED_SEARCH_PATH` 优先解析，**静默覆盖**新构建出来的 DLL，排查起来极其费时。

当前 `.gitignore` 已覆盖：

| 规则 | 挡住什么 |
|---|---|
| `hid-lib/build/` | 核心库产物 |
| `hid-lib/*/build/` | 各测试工程产物（含 `test_hid_controller/build/`） |
| `hid-py/build/`、`hid-py/dist/`、`hid-py/temp/`、`hid-py/*.egg-info/` | Python 构建中间物 |
| `hid-py/hid_controller/*.pyd` | 扩展模块 |
| `hid-py/hid_controller/*.dll` | 随行 DLL，正是当年那个坑 |
| `hid-py/hid_controller/__pycache__/` | 字节码 |

提交前请确认 `git status` 里没有任何 `.dll` / `.pyd` / `.lib` / `.exp` / `.obj` / `.pdb`。

### 6.5 跨 DLL 边界按值返回 STL 类型

`get_mouse_delta()`（两个重载）返回 `std::pair<long,long>`、`get_pressed_keys()` 返回 `std::vector<uint16_t>`，都是**按值**跨 DLL 边界。这只在两侧使用**同一份共享 CRT** 时才成立。

| 约束 | 说明 |
|---|---|
| CRT 设置 | `hid_controller.vcxproj` 未显式设置 `RuntimeLibrary`，Release 走 MSVC 默认的 `MultiThreadedDLL`（`/MD`）；CMake 对 MSVC 的默认同为动态 CRT。两侧一致纯属默认值恰好吻合，**不要改动它**，也不要给任一侧改成 `/MT` |
| 编译器与标准 | 两侧都必须是同一套 MSVC + C++17（`.vcxproj` 为 `stdcpp17`，`CMakeLists.txt` 为 `CMAKE_CXX_STANDARD 17` + `/std:c++17`） |
| 调用方限制 | 只能从链接了 `hid_controller.lib` 的扩展模块（或同样用 MSVC 链接导入库的 C++ 代码）调用。**绝不可**通过 ctypes、`GetProcAddress` 或其他语言 FFI 调用这三个函数 |

---

## 7. 验证清单

改完绑定层后逐条执行。以下结果均为本机实测值。

| # | 检查项 | 期望结果 | 实测 |
|---|---|---|---|
| 1 | `import hid_controller` | 不抛异常 | 通过 |
| 2 | `len(hid_send.get_key_code_map())` | 120 | 120 |
| 3 | `len(hid_send.get_mouse_button_map())` | 5 | 5 |
| 4 | `__all__` 中每个名字都能 `getattr` 到 | 无不可达项（`__all__` 共 29 项） | 全部可达 |
| 5 | `get_mouse_delta()` | 返回二元组不崩溃 | `(0, 0)` |
| 6 | `get_pressed_keys()` | 返回 list 不崩溃 | `[]` |
| 7 | `get_mouse_move_coefficient()` | 返回 float | `1.0` |
| 8 | `is_tracking()` 在 start / stop 前后翻转 | `False` → `True` → `False` | 与期望一致 |
| 9 | `mouse_down(object())` | 抛异常而非静默成功 | `TypeError` |
| 10 | `key_down(object())` | 抛异常而非静默成功 | `TypeError` |
| 11 | `start_input_tracking(None, cb, cb, cb)` | 抛异常（Python 侧不允许 `None`） | `TypeError` |
| 12 | 别名 `start_tracking` / `stop_tracking` 可达 | 可达 | 通过 |

### 7.1 无法在无头环境验证的部分

| 项目 | 原因 |
|---|---|
| 真实物理鼠标增量 | RAW INPUT 需要真实窗口句柄和消息循环。`get_mouse_delta()` 在无人操作时恒为 `(0,0)`，只能证明「调用不崩溃」，不能证明「数值正确」 |
| 真实按键集合 | 同上，`get_pressed_keys()` 无人按键时恒为 `[]` |

因此验证时**只调查询类函数**（`is_tracking` / `get_mouse_delta` / `get_pressed_keys` / `get_mouse_move_coefficient` 以及两个枚举映射）。**绝不要**在测试里调用会真实移动鼠标或按下按键的函数（`mouse_move_*`、`mouse_click`、`key_press`、`key_combo`、`key_seq` 等），更不要调用 `begin_key_intercept`，理由见 6.3。

### 7.2 一个值得知道的枚举计数差异

| 表达式 | 值 |
|---|---|
| `len(hid_send.get_key_code_map())` | 120 |
| `len(KeyCode.__members__)` | 120 |
| `len(KeyCode)` | **117** |

差 3 不是 bug。Python 的 `IntEnum` 会把值相同的成员折叠成别名，`len(KeyCode)` 只数规范成员。头文件里有三组有意的同值定义，先出现的成为规范名：

| 别名 | 规范成员 | 值 |
|---|---|---|
| `LSHIFT` | `SHIFT` | `0xE1` |
| `LCTRL` | `CTRL` | `0xE0` |
| `LALT` | `ALT` | `0xE2` |

实测 `KeyCode.__members__["LSHIFT"].name` 返回 `"SHIFT"`。这也意味着**遍历 `KeyCode` 拿不到 `LSHIFT` / `LCTRL` / `LALT` 这三个名字**，需要它们时请走 `KeyCode.__members__` 或 `KeyCode["LSHIFT"]`。
