# ==============================================================================
# cpp-sc2 - StarCraft II C++ Library
# ==============================================================================

cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(cpp-sc2
        VERSION 3.0.0
        DESCRIPTION "C++ library for creating StarCraft II bots"
        LANGUAGES CXX)

# ==============================================================================
# C++23 Standard and Compiler Requirements
# ==============================================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Check compiler versions for C++23 support
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14.0")
        message(FATAL_ERROR "GCC 14+ required for C++23 support")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18.0")
        message(FATAL_ERROR "Clang 18+ required for C++23 support")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.40")
        message(FATAL_ERROR
                "MSVC 19.38+ (Visual Studio 2022 17.8+) required for C++23 support.\n"
                "Current version: ${CMAKE_CXX_COMPILER_VERSION}\n"
                "Please update Visual Studio to version 17.8 or later.")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "15.0")
        message(FATAL_ERROR
                "Apple Clang 15+ required for C++23 support.\n"
                "Current version: ${CMAKE_CXX_COMPILER_VERSION}\n"
                "Please update Xcode.")
    endif()
endif()

# ==============================================================================
# Build Configuration
# ==============================================================================

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose build type" FORCE)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Enable modern CMake features
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)  # Enable LTO when available

# ==============================================================================
# Compiler Settings
# ==============================================================================

# Common compile definitions
add_compile_definitions(
        $<$<CONFIG:Debug>:DEBUG=1>
        $<$<CONFIG:Release>:NDEBUG=1>
        SC2_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        SC2_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        SC2_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Windows-specific settings
if(WIN32)
    add_compile_definitions(
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            _CRT_SECURE_NO_WARNINGS
    )
endif()

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
            -Wall -Wextra -Wpedantic
            -Wconversion -Wsign-conversion
    )
    add_compile_options($<$<CONFIG:Debug>:-g>)
    add_compile_options($<$<CONFIG:Debug>:-O0>)
    add_compile_options($<$<CONFIG:Release>:-O3>)
    add_compile_options($<$<CONFIG:Release>:-DNDEBUG>)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(
            /W4 /permissive- /utf-8
    )
    add_compile_options($<$<CONFIG:Debug>:/Od>)
    add_compile_options($<$<CONFIG:Debug>:/Zi>)
    add_compile_options($<$<CONFIG:Release>:/O2>)
    add_compile_options($<$<CONFIG:Release>:/DNDEBUG>)
endif()

# ==============================================================================
# Dependencies
# ==============================================================================

# Find required packages
find_package(Protobuf CONFIG REQUIRED)
find_package(ixwebsocket CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)

# Optional: Testing
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    find_package(GTest CONFIG REQUIRED)
    enable_testing()
endif()

# ==============================================================================
# Project Structure
# ==============================================================================

set(SC2_VERSION "5.0.14" CACHE STRING "Version of the target StarCraft II client")
set(CMAKE_DEBUG_POSTFIX "d")

add_subdirectory(src)

# Optional subdirectories
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples")
    option(BUILD_EXAMPLES "Build examples" ON)
    if(BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()

if(BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    add_subdirectory(tests)
endif()

# ==============================================================================
# Installation Targets
# ==============================================================================

install(TARGETS sc2utils sc2api sc2protocol
        EXPORT cpp-sc2Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)


# ==============================================================================
# Status Summary
# ==============================================================================

message(STATUS "cpp-sc2 ${PROJECT_VERSION} - SC2${SC2_VERSION} - C++${CMAKE_CXX_STANDARD} - ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
