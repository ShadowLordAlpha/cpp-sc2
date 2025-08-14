# ==============================================================================
# cpp-sc2 Library Components
# ==============================================================================

# Build subprojects in dependency order
add_subdirectory(sc2protocol)   # Depends on external protobuf only
add_subdirectory(sc2utils)      # No dependencies
add_subdirectory(sc2api)        # Depends on sc2utils + sc2protocol

# ==============================================================================
# Convenience Target
# ==============================================================================

# Create a convenience target that links all components
add_library(cpp-sc2 INTERFACE)
add_library(cppsc2::cpp-sc2 ALIAS cpp-sc2)

target_link_libraries(cpp-sc2 INTERFACE
        cppsc2::sc2utils
        cppsc2::sc2protocol
        cppsc2::sc2api
)

# ==============================================================================
# Package Export Configuration
# ==============================================================================

# Install targets and export them
install(TARGETS cpp-sc2 sc2utils sc2protocol sc2api
        EXPORT cpp-sc2-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        FILE_SET CXX_MODULES DESTINATION module
        INCLUDES DESTINATION include
)

# Install headers
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Export targets for other CMake projects
install(EXPORT cpp-sc2-targets
        FILE cpp-sc2-targets.cmake
        NAMESPACE cppsc2::
        DESTINATION lib/cmake/cpp-sc2
)

# ==============================================================================
# Status Summary
# ==============================================================================

message(STATUS "cpp-sc2 libraries configured:")
message(STATUS "  sc2utils: Cross-platform utilities")
message(STATUS "  sc2protocol: Protocol buffer definitions")
message(STATUS "  sc2api: Core StarCraft II API")
