cmake_minimum_required(VERSION 3.19)
project(HIDControllerPython LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure UTF-8 source and execution encoding for MSVC
if(MSVC)
    add_compile_options(/utf-8 /std:c++17)
endif()

# Find Python
find_package(Python COMPONENTS Interpreter Development REQUIRED)

# Fetch pybind11
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 2.12 CONFIG QUIET)
if(NOT pybind11_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.13.6
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# Directories
set(HIDCTL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../library")
set(HIDCTL_INCLUDE_DIR "${HIDCTL_LIB_DIR}/include")
set(HIDCTL_DLL_DIR "${HIDCTL_LIB_DIR}/build/Release")
set(PKG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/hid_controller")

# ==================== _extension module ====================
pybind11_add_module(_extension
    "${HIDCTL_LIB_DIR}/src/bindings/main_bindings.cpp"
)

target_include_directories(_extension PRIVATE ${HIDCTL_INCLUDE_DIR})

# Link against HIDController.lib if available (DLL already built)
if(EXISTS "${HIDCTL_DLL_DIR}/HIDController.lib")
    target_link_libraries(_extension PRIVATE "${HIDCTL_DLL_DIR}/HIDController.lib")
endif()

# Copy DLL next to .pyd
if(EXISTS "${HIDCTL_DLL_DIR}/HIDController.dll")
    add_custom_command(TARGET _extension POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${HIDCTL_DLL_DIR}/HIDController.dll"
            $<TARGET_FILE_DIR:_extension>
        COMMENT "Copying HIDController.dll next to _extension.pyd"
    )
endif()

# ==================== input_tracker module ====================
pybind11_add_module(input_tracker
    "${HIDCTL_LIB_DIR}/src/bindings/input_tracker_bindings.cpp"
    "${HIDCTL_LIB_DIR}/src/core/input_tracker_impl.cpp"
)

target_include_directories(input_tracker PRIVATE ${HIDCTL_INCLUDE_DIR} "${HIDCTL_LIB_DIR}/src/core")

if(EXISTS "${HIDCTL_DLL_DIR}/HIDController.lib")
    target_link_libraries(input_tracker PRIVATE "${HIDCTL_DLL_DIR}/HIDController.lib")
endif()

if(EXISTS "${HIDCTL_DLL_DIR}/HIDController.dll")
    add_custom_command(TARGET input_tracker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${HIDCTL_DLL_DIR}/HIDController.dll"
            $<TARGET_FILE_DIR:input_tracker>
        COMMENT "Copying HIDController.dll next to input_tracker.pyd"
    )
endif()

# Copy .pyd files into hid_controller/ package directory
add_custom_target(copy_pydstamp ALL DEPENDS _extension input_tracker)
add_custom_command(TARGET copy_pydstamp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:_extension>
        ${PKG_DIR}/_extension.pyd
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:input_tracker>
        ${PKG_DIR}/input_tracker.pyd
    COMMENT "Copying .pyd files into hid_controller/ package"
)

# Install rules
install(TARGETS _extension input_tracker
    DESTINATION hid_controller
)
