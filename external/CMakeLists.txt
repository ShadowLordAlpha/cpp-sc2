# ==============================================================================
# external - third-party code that cpp-sc2 depends on
# ==============================================================================

set(protobuf_BUILD_TESTS OFF CACHE BOOL "Disable protobuf tests")
set(protobuf_WITH_ZLIB OFF CACHE BOOL "Disable zlib in protobuf")
set(protobuf_BUILD_PROTOC_BINARIES ON CACHE BOOL "Build protoc executable")
set(protobuf_VERBOSE ON CACHE BOOL "")
add_subdirectory(protobuf EXCLUDE_FROM_ALL)


# ==============================================================================
# Build s2client-proto files using protobuf into something usable for cpp-sc2
# ==============================================================================

# Set the path to the s2client-proto submodule
set(SC2PROTO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/s2client-proto/")

# Set the path to where the generated code will be
set(SC2PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/sc2proto")

# Collect the Proto files.
file(GLOB_RECURSE SC2PROTO_PROTO_FILES
        "${SC2PROTO_ROOT}s2clientprotocol/*.proto"
)

if(NOT SC2PROTO_PROTO_FILES)
    message(FATAL_ERROR "No .proto files found under: ${SC2PROTO_ROOT}")
endif()

# Group the generated sources into a static library that can be reused elsewhere
add_library(sc2proto STATIC)
set(SC2PROTO_OUTPOUT)
include(${CMAKE_CURRENT_SOURCE_DIR}/protobuf/cmake/protobuf-generate.cmake) # TODO: find a better way of including this cmake
protobuf_generate(
        TARGET sc2proto
        OUT_VAR SC2PROTO_OUTPOUT
        LANGUAGE cpp
        PROTOC_OUT_DIR ${SC2PROTO_GEN_DIR}
        IMPORT_DIRS ${SC2PROTO_ROOT}
        PROTOS ${SC2PROTO_PROTO_FILES}
)

# Public interface:
# - Consumers need libprotobuf and the generated headers include dir
# - Private dependency on the protobuf library itself for our compilation
target_link_libraries(sc2proto
        PUBLIC
        protobuf::libprotobuf
)

target_include_directories(sc2proto
        PUBLIC
        "${SC2PROTO_GEN_DIR}"      # so other targets can include the generated headers
)

# alias for convenience in other CMakeLists
add_library(cpp-sc2::sc2proto ALIAS sc2proto)

# Example usage in other modules:
# target_link_libraries(your_target PRIVATE cpp-sc2::sc2proto)
