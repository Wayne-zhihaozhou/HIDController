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

# ==================== hid_send module ====================
pybind11_add_module(hid_send
    "${HIDCTL_LIB_DIR}/src/bindings/hid_send_bindings.cpp"
)

target_include_directories(hid_send PRIVATE ${HIDCTL_INCLUDE_DIR})

# Link against HIDController.lib if available (DLL already built)
if(EXISTS "${HIDCTL_DLL_DIR}/HIDController.lib")
    target_link_libraries(hid_send PRIVATE "${HIDCTL_DLL_DIR}/HIDController.lib")
endif()

# Copy DLL next to .pyd
if(EXISTS "${HIDCTL_DLL_DIR}/HIDController.dll")
    add_custom_command(TARGET hid_send POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${HIDCTL_DLL_DIR}/HIDController.dll"
            $<TARGET_FILE_DIR:hid_send>
        COMMENT "Copying HIDController.dll next to hid_send.pyd"
    )
endif()

# ==================== raw_input module ====================
pybind11_add_module(raw_input
    "${HIDCTL_LIB_DIR}/src/bindings/raw_input_bindings.cpp"
    "${HIDCTL_LIB_DIR}/src/core/input_tracker_impl.cpp"
)

target_include_directories(raw_input PRIVATE ${HIDCTL_INCLUDE_DIR} "${HIDCTL_LIB_DIR}/src/core")

if(EXISTS "${HIDCTL_DLL_DIR}/HIDController.lib")
    target_link_libraries(raw_input PRIVATE "${HIDCTL_DLL_DIR}/HIDController.lib")
endif()

if(EXISTS "${HIDCTL_DLL_DIR}/HIDController.dll")
    add_custom_command(TARGET raw_input POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${HIDCTL_DLL_DIR}/HIDController.dll"
            $<TARGET_FILE_DIR:raw_input>
        COMMENT "Copying HIDController.dll next to raw_input.pyd"
    )
endif()

# Copy .pyd files into hid_controller/ package directory
add_custom_target(copy_pydstamp ALL DEPENDS hid_send raw_input)
add_custom_command(TARGET copy_pydstamp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:hid_send>
        ${PKG_DIR}/hid_send.pyd
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raw_input>
        ${PKG_DIR}/raw_input.pyd
    COMMENT "Copying .pyd files into hid_controller/ package"
)

# Install rules
install(TARGETS hid_send raw_input
    DESTINATION hid_controller
)
