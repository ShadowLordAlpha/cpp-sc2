# ==============================================================================
# sc2api - Core API Library for cpp-sc2
# ==============================================================================

# ==============================================================================
# Version-Specific Source Generation
# ==============================================================================

# Inject sources specific for particular version of the game client
set(TYPEENUMS_HEADER "${PROJECT_SOURCE_DIR}/include/sc2api/sc2_typeenums.h")
file(WRITE "${TYPEENUMS_HEADER}"
"#pragma once

#include \"typeids/sc2_${SC2_VERSION}_typeenums.h\"
"
)

# ==============================================================================
# Library Sources
# ==============================================================================

# Core API sources
set(SC2API_SOURCES
        sc2_agent.cc
        sc2_args.cc
        sc2_client.cc
        sc2_connection.cc
        sc2_coordinator.cc
        sc2_data.cc
        sc2_game_settings.cc
        sc2_map_info.cpp
        sc2_proto_interface.cc
        sc2_proto_to_pods.cc
        sc2_replay_observer.cc
        sc2_score.cc
        sc2_server.cc
        sc2_unit.cc
)

# Add version-specific type enums implementation
list(APPEND SC2API_SOURCES "typeids/sc2_${SC2_VERSION}_typeenums.cpp")

# ==============================================================================
# Library Target
# ==============================================================================

# Create the static library
add_library(sc2api STATIC ${SC2API_SOURCES})

# Create alias for consistent naming
add_library(cppsc2::sc2api ALIAS sc2api)

# ==============================================================================
# Include Directories
# ==============================================================================

target_include_directories(sc2api
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

# ==============================================================================
# Dependencies
# ==============================================================================

# Find required packages
find_package(Threads REQUIRED)
find_package(ixwebsocket CONFIG REQUIRED)

# Link dependencies
target_link_libraries(sc2api
        PUBLIC
            # Public dependencies that consumers need
            cppsc2::sc2protocol
            cppsc2::sc2utils
            Threads::Threads
        PRIVATE
            # Private dependencies only used internally
            ixwebsocket::ixwebsocket
)

# ==============================================================================
# Status Summary
# ==============================================================================

message(STATUS "sc2api configuration:")
message(STATUS "  SC2 Version: ${SC2_VERSION}")
message(STATUS "  Generated header: ${TYPEENUMS_HEADER}")
message(STATUS "  C++ Standard: C++${CMAKE_CXX_STANDARD}")

# Show version-specific typeenums file being used
message(STATUS "  Type enums source: typeids/sc2_${SC2_VERSION}_typeenums.cpp")

# List core source files
message(STATUS "  Core source files:")
foreach(source ${SC2API_SOURCES})
    if(NOT source MATCHES "typeids/")
        message(STATUS "    ${source}")
    endif()
endforeach()

# Validate that the version-specific file exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/typeids/sc2_${SC2_VERSION}_typeenums.cpp")
    message(WARNING "Version-specific typeenums file not found: typeids/sc2_${SC2_VERSION}_typeenums.cpp")
    message(STATUS "Available versions:")
    file(GLOB AVAILABLE_VERSIONS "${CMAKE_CURRENT_SOURCE_DIR}/typeids/sc2_*.cpp")
    foreach(version_file ${AVAILABLE_VERSIONS})
        get_filename_component(version_name ${version_file} NAME_WE)
        string(REGEX REPLACE "sc2_(.+)_typeenums" "\\1" version_number ${version_name})
        message(STATUS "    ${version_number}")
    endforeach()
endif()
