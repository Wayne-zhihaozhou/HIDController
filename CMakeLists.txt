# HIDController DLL 项目配置
# 用于编译生成独立的 HIDController.dll 和 HIDController.lib

cmake_minimum_required(VERSION 3.16)
project(HIDController LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录 - 所有文件输出到同一分发目录
set(DIST_DIR ${CMAKE_BINARY_DIR}/dist)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIST_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIST_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIST_DIR})

# 定义导出符号宏
add_definitions(-DDLL1_EXPORTS)

# 查找所有 C++ 核心源文件
file(GLOB HID_CONTROLLER_SOURCES
    "src/core/*.cpp"
)

# 排除 pch.cpp（预编译头）和 input_tracker.cpp（RAW INPUT，不用于纯 DLL 构建）
list(FILTER HID_CONTROLLER_SOURCES EXCLUDE REGEX ".*pch\\.cpp$")
list(FILTER HID_CONTROLLER_SOURCES EXCLUDE REGEX ".*input_tracker\\.cpp$")

# 创建 DLL 目标
add_library(HIDController SHARED
    ${HID_CONTROLLER_SOURCES}
)

# 包含头文件目录
target_include_directories(HIDController PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 链接必要的 Windows 库
target_link_libraries(HIDController
    user32.lib
    kernel32.lib
)

# 设置预编译头
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
    target_precompile_header(HIDController PRIVATE include/pch.h)
    source_group("Header Files" FILES include/pch.h)
endif()

# 设置编译选项
if(MSVC)
    target_compile_options(HIDController PRIVATE 
        /W4
        /WX-
        /permissive-
    )
endif()

# 设置导入库和 DLL 的输出名称
set_target_properties(HIDController PROPERTIES
    OUTPUT_NAME "HIDController"
    RUNTIME_OUTPUT_DIRECTORY "${DIST_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${DIST_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${DIST_DIR}"
)

# 安装规则
install(TARGETS HIDController
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)
