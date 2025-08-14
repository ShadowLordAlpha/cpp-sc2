# ==============================================================================
# cpp-sc2 - StarCraft II C++ Library
# ==============================================================================

cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(cpp-sc2
        VERSION 3.0.0
        DESCRIPTION "C++ library for creating StarCraft II bots"
        LANGUAGES CXX)

# ==============================================================================
# Global Settings
# ==============================================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Common options
option(BUILD_EXAMPLES "Build example programs" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})

# StarCraft II target client version
set(SC2_VERSION "5.0.14" CACHE STRING "Version of the target StarCraft II client")

# ==============================================================================
# Project structure
# ==============================================================================

# Third-party and vendor code (protobuf, s2client-proto, etc.) must be handled inside this directory
add_subdirectory(external)

# Main library and internal targets
add_subdirectory(src)

# Optional: examples
if(BUILD_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples")
    add_subdirectory(examples)
endif()

# Optional: tests
if(BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    enable_testing()
    add_subdirectory(tests)
endif()

# ==============================================================================
# Status summary
# ==============================================================================

message(STATUS "cpp-sc2 configuration:")
message(STATUS "  Version:            ${PROJECT_VERSION}")
message(STATUS "  SC2 Target:         ${SC2_VERSION}")
message(STATUS "  C++ Standard:       C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type:         ${CMAKE_BUILD_TYPE}")
message(STATUS "  Examples:           ${BUILD_EXAMPLES}")
message(STATUS "  Tests:              ${BUILD_TESTS}")
