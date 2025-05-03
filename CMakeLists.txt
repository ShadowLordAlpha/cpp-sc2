cmake_minimum_required(VERSION 3.15)

# Project Settings and C++ Standard
project(cpp-sc2 VERSION 3.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set CMake Policies
if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    cmake_policy(SET CMP0135 NEW)
endif ()

# User Options
option(BUILD_SC2_RENDERER "Build SC2 Renderer library" ON)
option(BUILD_API_EXAMPLES "Build Examples" ON)
option(BUILD_API_TESTS "Build Tests" ON)
set(SC2_VERSION "5.0.14" CACHE STRING "Version of the target StarCraft II client")
set(CMAKE_DEBUG_POSTFIX "d")

message(STATUS "Target SC2 version: ${SC2_VERSION}")

include(FetchContent)

# External subprojects
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/thirdparty/cmake")

# Allow creating filters for projects in visual studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# [Optional] MSVC-specific options (not currently needed with vcpkg/toolchain)
#if (MSVC)
    # Setup MSVC parallelized builds
#    add_compile_options(/MP)
#    add_compile_options(/MDd) # There is a really dumb assert that happens when not using this

    # Use statically linked runtime
#    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
#endif ()

find_package(spdlog CONFIG REQUIRED)

if (BUILD_API_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (BUILD_API_TESTS)
    add_subdirectory(tests)
endif ()

# External subproject dependencies
include(sc2protocol)

# cpp-sc2 libraries
add_subdirectory(src)
