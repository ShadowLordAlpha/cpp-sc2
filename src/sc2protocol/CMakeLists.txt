
# ==============================================================================
# sc2protocol - Protocol Buffer Library for cpp-sc2
# ==============================================================================

# Fetch s2client-proto sources
include(FetchContent)

if(NOT sc2protocol_SOURCE_DIR)
    message(STATUS "Fetching s2client-proto sources via FetchContent...")
    FetchContent_Declare(
            sc2proto
            GIT_REPOSITORY https://github.com/Blizzard/s2client-proto.git
            GIT_TAG a6d60f4cce7886a3be2bf7af2c1b12fdab6e9c63
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(sc2proto)
endif()

# ==============================================================================
# Dependencies
# ==============================================================================

find_package(Protobuf CONFIG REQUIRED)

# ==============================================================================
# Protocol Buffer Setup
# ==============================================================================

# Set up proto file locations
set(PROTO_DIR "${sc2proto_SOURCE_DIR}")
file(GLOB PROTO_FILES "${PROTO_DIR}/s2clientprotocol/*.proto")

# Generated code destination
set(PROTO_GEN_DIR "${PROJECT_BINARY_DIR}/generated/s2clientprotocol")

# Validate proto files exist
if(NOT PROTO_FILES)
    message(FATAL_ERROR "No *.proto files found in ${sc2proto_SOURCE_DIR}/s2clientprotocol. Check path.")
endif()

# Ensure CMake reconfigures if proto files change
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${PROTO_DIR}/*.proto")

# ==============================================================================
# Library Target
# ==============================================================================

# Create the static library
add_library(sc2protocol STATIC ${PROTO_FILES})

# Create alias for consistent naming
add_library(cppsc2::sc2protocol ALIAS sc2protocol)

# ==============================================================================
# Include Directories
# ==============================================================================

target_include_directories(sc2protocol
        PUBLIC
            $<BUILD_INTERFACE:${PROTO_GEN_DIR}>
)

# ==============================================================================
# Dependencies
# ==============================================================================

target_link_libraries(sc2protocol
        PUBLIC
            # Public dependencies that consumers need
            protobuf::libprotobuf
)

# ==============================================================================
# Protocol Buffer Generation
# ==============================================================================

# Ensure output directory exists
file(MAKE_DIRECTORY "${PROTO_GEN_DIR}")

# Generate C++ code from .proto files
protobuf_generate(
        TARGET sc2protocol
        IMPORT_DIRS "${PROTO_DIR}"
        PROTOC_OUT_DIR "${PROTO_GEN_DIR}"
)

# ==============================================================================
# Status Summary
# ==============================================================================

message(STATUS "sc2protocol configuration:")
message(STATUS "  Protocol source: ${PROTO_DIR}")
message(STATUS "  Generated output: ${PROTO_GEN_DIR}")
message(STATUS "  Protobuf version: ${Protobuf_VERSION}")

# List proto files being processed
message(STATUS "  Proto files:")
foreach(proto_file ${PROTO_FILES})
    get_filename_component(proto_name ${proto_file} NAME)
    message(STATUS "    ${proto_name}")
endforeach()
