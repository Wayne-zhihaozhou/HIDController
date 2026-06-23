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
set(PKG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dist")

# Output: .pyd goes to dist/; .lib/.exp go to temp/
set(DIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dist")
set(TEMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/temp")
file(MAKE_DIRECTORY "${DIST_DIR}")
file(MAKE_DIRECTORY "${TEMP_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${DIST_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${DIST_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${TEMP_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${DIST_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${DIST_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${TEMP_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${TEMP_DIR}")

# ==================== hid_send module ====================
Python_add_library(hid_send MODULE WITH_SOABI
    "${HIDCTL_LIB_DIR}/src/bindings/hid_send_bindings.cpp"
)

target_include_directories(hid_send PRIVATE ${HIDCTL_INCLUDE_DIR})
target_compile_definitions(hid_send PRIVATE HID_CONTROLLER_EXPORTS)

# Override Python_add_library's output dirs to point to dist/
set_target_properties(hid_send PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${DIST_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${DIST_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${DIST_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${TEMP_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${TEMP_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${TEMP_DIR}"
)

# Link against hid_controller.lib if available
if(EXISTS "${HIDCTL_DLL_DIR}/hid_controller.lib")
    target_link_libraries(hid_send PRIVATE "${HIDCTL_DLL_DIR}/hid_controller.lib")
endif()

# Copy DLL next to .pyd
if(EXISTS "${HIDCTL_DLL_DIR}/hid_controller.dll")
    add_custom_command(TARGET hid_send POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${HIDCTL_DLL_DIR}/hid_controller.dll"
            $<TARGET_FILE_DIR:hid_send>
        COMMENT "Copying hid_controller.dll next to hid_send.pyd"
    )
endif()

# ==================== raw_input module ====================
Python_add_library(raw_input MODULE
    "${HIDCTL_LIB_DIR}/src/bindings/raw_input_bindings.cpp"
    "${HIDCTL_LIB_DIR}/src/core/input_tracker_impl.cpp"
    WITH_SOABI
)
target_include_directories(raw_input PRIVATE ${HIDCTL_INCLUDE_DIR} "${HIDCTL_LIB_DIR}/src/core")

set_target_properties(raw_input PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${DIST_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${DIST_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${DIST_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${TEMP_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${TEMP_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${TEMP_DIR}"
)

if(EXISTS "${HIDCTL_DLL_DIR}/hid_controller.lib")
    target_link_libraries(raw_input PRIVATE "${HIDCTL_DLL_DIR}/hid_controller.lib")
endif()

if(EXISTS "${HIDCTL_DLL_DIR}/hid_controller.dll")
    add_custom_command(TARGET raw_input POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${HIDCTL_DLL_DIR}/hid_controller.dll"
            $<TARGET_FILE_DIR:raw_input>
        COMMENT "Copying hid_controller.dll next to raw_input.pyd"
    )
endif()

# Copy .pyd files into dist/ directory
add_custom_target(copy_pydstamp ALL DEPENDS hid_send raw_input)
add_custom_command(TARGET copy_pydstamp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:hid_send>
        ${PKG_DIR}/hid_send.pyd
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raw_input>
        ${PKG_DIR}/raw_input.pyd
    COMMENT "Copying .pyd files into dist/ package"
)


# Install rules
install(TARGETS hid_send raw_input
    DESTINATION dist
)
