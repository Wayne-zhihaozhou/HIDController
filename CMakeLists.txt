# HIDController DLL 项目配置
# 用于编译生成独立的 HIDController.dll 和 HIDController.lib

cmake_minimum_required(VERSION 3.15)
project(HIDController LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 定义导出符号宏
add_definitions(-DDLL1_EXPORTS)

# 查找所有源文件
file(GLOB HID_CONTROLLER_SOURCES
    "src/*.cpp"
)

# 排除 pch.cpp（它只用于预编译头）
list(FILTER HID_CONTROLLER_SOURCES EXCLUDE REGEX ".*pch\\.cpp$")

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
target_precompile_header(HIDController PRIVATE include/pch.h)
source_group("Header Files" FILES include/pch.h)

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
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 安装规则
install(TARGETS HIDController
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)
