# ==============================================================================
# cpp-sc2 - StarCraft II C++ Library
# ==============================================================================

cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(astra
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
option(ASTRA_BUILD_EXAMPLES "Build example programs" ${PROJECT_IS_TOP_LEVEL})
option(ASTRA_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})

# Ensure dynamic runtime for MSVC
if (MSVC)
    # Set runtime library to MDd (dynamic debug) for Debug builds
    # and MD (dynamic release) for Release builds
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# StarCraft II target client version
set(SC2_VERSION "5.0.14" CACHE STRING "Version of the target StarCraft II client")

# ==============================================================================
# Project structure
# ==============================================================================

# Third-party and vendor code (protobuf, s2client-proto, etc.) must be handled inside this directory
add_subdirectory(external)
find_package(ixwebsocket CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# Integrations to specific games should be handled inside this directory
add_subdirectory(extras)

add_library(astra STATIC

        include/astra/process.h
        include/astra/websocket.h
        include/astra/coordinator.h
        include/astra/game_state.h
        include/astra.h
        include/astra/agent.h
        include/astra/game.h
        include/astra/action.h

        src/astra/process.cpp
        src/astra/websocket.cpp
)
target_include_directories(astra
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
)

target_link_libraries(astra PUBLIC glm::glm)

target_link_libraries(astra PRIVATE ixwebsocket::ixwebsocket)
target_link_libraries(astra PRIVATE spdlog::spdlog)

# alias for convenience in other CMakeLists
add_library(astra::astra ALIAS astra)

# Optional: examples
if(ASTRA_BUILD_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples")
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

message(STATUS "astra configuration:")
message(STATUS "  Version:            ${PROJECT_VERSION}")
message(STATUS "  SC2 Target:         ${SC2_VERSION}")
message(STATUS "  C++ Standard:       C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type:         ${CMAKE_BUILD_TYPE}")
message(STATUS "  Examples:           ${ASTRA_BUILD_EXAMPLES}")
message(STATUS "  Tests:              ${ASTRA_BUILD_TESTS}")
