cmake_minimum_required(VERSION 3.18)
project(hid_controller LANGUAGES CXX)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
set(SOURCES
    hid-lib/src/core/hid_raw_input_dll.cpp
    hid-lib/src/core/ib_send_keyboard.cpp
    hid-lib/src/core/ib_send_mouse.cpp
    hid-lib/src/core/input_tracker_impl.cpp
    hid-lib/src/core/logitech.cpp
    hid-lib/src/core/logitech_driver.cpp
)

# Header files
set(HEADERS
    hid-lib/include/hid_controller.h
    hid-lib/include/logitech.h
    hid-lib/include/logitech_driver.h
    hid-lib/include/mouse_button.h
    hid-lib/include/pch.h
    hid-lib/include/virtual_keys.h
)

# Build DLL
add_library(hid_controller SHARED ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(hid_controller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hid-lib/include)

# DLL export symbol
target_compile_definitions(hid_controller PRIVATE hid_controller_EXPORTS)

# Windows subsystem
target_link_options(hid_controller PRIVATE /SUBSYSTEM:CONSOLE)

# Output to build/Release/ directory
set_target_properties(hid_controller PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/hid-lib/build"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/hid-lib/build"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/hid-lib/build"
)

# Installation (optional)
install(TARGETS hid_controller
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
)
install(FILES ${HEADERS} DESTINATION include)
