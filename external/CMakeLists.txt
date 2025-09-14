# ==============================================================================
# external - third-party code that cpp-sc2 depends on
# ==============================================================================

set(protobuf_BUILD_TESTS OFF CACHE BOOL "Disable protobuf tests")
set(protobuf_WITH_ZLIB OFF CACHE BOOL "Disable zlib in protobuf")
set(protobuf_BUILD_PROTOC_BINARIES ON CACHE BOOL "Build protoc executable")
add_subdirectory(protobuf EXCLUDE_FROM_ALL)

add_subdirectory(glm)

# Example usage in other modules:
# target_link_libraries(your_target PRIVATE cpp-sc2::sc2proto)
