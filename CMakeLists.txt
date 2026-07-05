cmake_minimum_required(VERSION 3.18)
project(HIDController LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/IbSendKeyboard.cpp
    src/IbSendMouse.cpp
    src/Logitech.cpp
    src/LogitechDriver.cpp
    src/pch.cpp
)

# Header files
set(HEADERS
    include/HIDController.hpp
    include/KeyboardMap.hpp
    include/Logitech.hpp
    include/LogitechDriver.hpp
    include/pch.h
)

# Build DLL
add_library(HIDController SHARED ${SOURCES} ${HEADERS})

# Precompiled header
target_precompile_header(HIDController PRIVATE include/pch.h)

# Define DLL export symbol
target_compile_definitions(HIDController PRIVATE DLL1_EXPORTS)

# Windows subsystem
target_link_options(HIDController PRIVATE /SUBSYSTEM:WINDOWS)

# Output to dll/ directory
set_target_properties(HIDController PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dll"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dll"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dll"
)

# Installation (optional)
install(TARGETS HIDController
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
)
install(FILES ${HEADERS} DESTINATION include)
