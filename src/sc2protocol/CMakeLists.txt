# ----------------------------------------------------------------------------
# Build logic for sc2protocol library (subdirectory)
#
# This CMake file:
#   - Compiles all .proto files in 's2clientprotocol' into C++ sources/headers with protobuf_generate_cpp()
#   - Builds a static library for the protocol
#   - Installs only the generated C++ headers, not the .proto files, and CMake export targets
#
# Requirements:
#   * Protobuf, found via find_package
#   * CMake >= 3.14 (for protobuf_generate_cpp TARGET argument)
#
# Exported target: cppsc2::sc2protocol
#
# Usage example in downstream CMake:
#   find_package(sc2protocol CONFIG REQUIRED)
#   target_link_libraries(your_target PRIVATE cppsc2::sc2protocol)
#
# NOTE: .proto files are NOT installed. Consume the generated C++ headers and link the library.
#       If you wish to regenerate code, please clone and build this project directly.
#
# For more details or troubleshooting, see the main README.md or docs/build.md.
# ----------------------------------------------------------------------------

# -------------------------------
# Fetch Proto files
# -------------------------------
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

# -------------------------------
# Dependencies
# -------------------------------
find_package(Protobuf CONFIG REQUIRED)

# -------------------------------
# Proto file setup
# -------------------------------
set(PROTO_DIR "${sc2proto_SOURCE_DIR}")
file(GLOB PROTO_FILES "${PROTO_DIR}/s2clientprotocol/*.proto")

# Generated code goes into build dir, namespaced by proto module
set(PROTO_GEN_DIR "${PROJECT_BINARY_DIR}/generated/s2clientprotocol")

if(NOT PROTO_FILES)
    message(FATAL_ERROR "No *.proto files found in ${sc2proto_SOURCE_DIR}/s2clientprotocol. Check path.")
endif()

# Ensure CMake reconfigures if proto file set changes.
# This avoids common GLOB pitfalls (new/removed files detected). See:
# https://cmake.org/cmake/help/latest/command/file.html#glob
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${PROTO_DIR}/*.proto")

# -------------------------------
# Library target
# -------------------------------
# Generate the library target before generating the cpp code from protobuf
add_library(sc2protocol STATIC OBJECT ${PROTO_FILES})

target_link_libraries(sc2protocol PUBLIC protobuf::libprotobuf)
target_include_directories(sc2protocol PUBLIC "$<BUILD_INTERFACE:${PROTO_GEN_DIR}>")

# -------------------------------
# Protobuf code generation
# -------------------------------
file(MAKE_DIRECTORY "${PROTO_GEN_DIR}")  # <-- Add this line here
protobuf_generate(
        TARGET sc2protocol              # Generated files will be associated with this target
        IMPORT_DIRS "${PROTO_DIR}"
        PROTOC_OUT_DIR "${PROTO_GEN_DIR}"
)

# Show what is being built in a clear fashion for CI/logs/developers
message(STATUS "sc2protocol: Added proto files:")
foreach(proto_file ${PROTO_FILES})
    message(STATUS "  ${proto_file}")
endforeach()
