cmake_minimum_required(VERSION 3.19)
project(hid_controller_python LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure UTF-8 source and execution encoding for MSVC
if(MSVC)
    add_compile_options(/utf-8 /std:c++17)
endif()

# Find Python
find_package(Python COMPONENTS Interpreter Development REQUIRED)

# Directories
set(HIDCTL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../hid-lib")
set(HIDCTL_INCLUDE_DIR "${HIDCTL_LIB_DIR}/include")
set(HIDCTL_DLL_DIR "${HIDCTL_LIB_DIR}/build/Release")

# The .pyd must land in the importable package directory. setup.py passes
# -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=<extdir>; a bare cmake invocation falls back
# to the in-tree package so `import hid_controller.hid_send` works either way.
if(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(PKG_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
else()
    set(PKG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/hid_controller")
endif()

# Import libraries / .exp of the extension modules are build litter, not package content
set(TEMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/temp")
file(MAKE_DIRECTORY "${PKG_DIR}")
file(MAKE_DIRECTORY "${TEMP_DIR}")

# The extension links the DLL's import library, so both must already be built.
if(NOT EXISTS "${HIDCTL_DLL_DIR}/hid_controller.lib")
    message(FATAL_ERROR "hid_controller.lib not found in ${HIDCTL_DLL_DIR} — build hid-lib (Release|x64) first")
endif()
if(NOT EXISTS "${HIDCTL_DLL_DIR}/hid_controller.dll")
    message(FATAL_ERROR "hid_controller.dll not found in ${HIDCTL_DLL_DIR} — build hid-lib (Release|x64) first")
endif()

# Visual Studio is a multi-config generator: without the per-config properties it
# appends Release/ to the output directory and the .pyd misses the package root.
function(hidctl_configure_module target)
    target_include_directories(${target} PRIVATE ${HIDCTL_INCLUDE_DIR})
    set_target_properties(${target} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PKG_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PKG_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${TEMP_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${TEMP_DIR}"
    )
    target_link_libraries(${target} PRIVATE "${HIDCTL_DLL_DIR}/hid_controller.lib")
    # CPython loads extensions with LOAD_WITH_ALTERED_SEARCH_PATH, so the DLL
    # beside the .pyd is the one resolved at import time.
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${HIDCTL_DLL_DIR}/hid_controller.dll"
            "$<TARGET_FILE_DIR:${target}>"
        COMMENT "Copying hid_controller.dll next to ${target}"
    )
endfunction()

# ==================== hid_send module ====================
Python_add_library(hid_send MODULE WITH_SOABI
    "${HIDCTL_LIB_DIR}/src/bindings/hid_send_bindings.cpp"
)
hidctl_configure_module(hid_send)

# ==================== raw_input module ====================
# input_tracker_impl.cpp is deliberately NOT compiled in: the tracker's file-scope
# state must live only in hid_controller.dll, otherwise this module would track
# into its own private copy and every query would read zeroes.
Python_add_library(raw_input MODULE WITH_SOABI
    "${HIDCTL_LIB_DIR}/src/bindings/raw_input_bindings.cpp"
)
hidctl_configure_module(raw_input)

# Install rules
install(TARGETS hid_send raw_input
    DESTINATION hid_controller
)
