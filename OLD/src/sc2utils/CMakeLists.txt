# ==============================================================================
# sc2utils - Utility Library for cpp-sc2
# ==============================================================================

# Setup library sources
set(SC2UTILS_SOURCES
        arg_parser.cpp
        platform.cpp
        sc2_utils.cpp
        ProcessManager.cpp
)

# Create the static library
add_library(sc2utils STATIC ${SC2UTILS_SOURCES})

# Create alias for consistent naming
add_library(cppsc2::sc2utils ALIAS sc2utils)


# ==============================================================================
# Include Directories
# ==============================================================================

target_include_directories(sc2utils
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/sc2utils>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/sc2api>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

# ==============================================================================
# Dependencies
# ==============================================================================

# Required threading support for cross-platform process management
find_package(Threads REQUIRED)

# Link dependencies
target_link_libraries(sc2utils
        PUBLIC
            # Public dependencies that consumers need
            Threads::Threads
        PRIVATE
            # Private dependencies only used internally
            spdlog::spdlog
)

# Platform-specific system libraries for process management
if(WIN32)
    target_link_libraries(sc2utils PRIVATE
            ws2_32      # Winsock for networking
            iphlpapi    # IP Helper API
            psapi       # Process Status API
    )
endif()

# ==============================================================================
# Status Summary
# ==============================================================================

message(STATUS "sc2utils configuration:")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")

# Show platform-specific implementation
if(WIN32)
    message(STATUS "  Process implementation: Windows")
else()
    message(STATUS "  Process implementation: POSIX")
endif()

# List source files being compiled
get_target_property(SC2UTILS_SOURCES sc2utils SOURCES)
message(STATUS "  Source files:")
foreach(source ${SC2UTILS_SOURCES})
    message(STATUS "    ${source}")
endforeach()
