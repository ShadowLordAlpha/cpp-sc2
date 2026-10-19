message(STATUS "FetchContent: civetweb")

# Disable builds
set(CIVETWEB_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(CIVETWEB_ENABLE_SERVER_EXECUTABLE OFF CACHE BOOL "" FORCE)

# Disable ASAN debug
set(CIVETWEB_ENABLE_ASAN OFF CACHE BOOL "" FORCE)

# Enable websocket connections
set(CIVETWEB_ENABLE_WEBSOCKETS ON CACHE BOOL "" FORCE)

# Disable IPv6 as we use only IPv4
set(CIVETWEB_ENABLE_IPV6 OFF CACHE BOOL "" FORCE)

# Apply source code tweaks: every *.patch in this directory. The patch step can run again on a source that is already
# patched (UPDATE_DISCONNECTED keeps it), so apply-patches.cmake applies them only when they are not in yet.
find_package(Git REQUIRED)
set(patch_executor
    ${CMAKE_COMMAND}
    "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
    "-DPATCH_DIR=${CMAKE_CURRENT_LIST_DIR}"
    -P "${CMAKE_CURRENT_LIST_DIR}/apply-patches.cmake"
)

if (APPLE)
    add_compile_options(
        -Wno-expansion-to-defined
        -Wno-extra-semi
        -Wno-macro-redefined
        -Wno-nullability-completeness
        -Wno-nullability-extension
        -Wno-used-but-marked-unused
    )
endif ()

FetchContent_Declare(
    civetweb
    GIT_REPOSITORY https://github.com/civetweb/civetweb.git
    GIT_TAG dfdfdb6926765541751cab055472f70daa9aa2dd
    GIT_PROGRESS TRUE
    PATCH_COMMAND ${patch_executor}
    UPDATE_DISCONNECTED TRUE
)
FetchContent_MakeAvailable(civetweb)

set_target_properties(civetweb-c-library PROPERTIES FOLDER contrib)

target_compile_options(civetweb-c-library PUBLIC -DUSE_IPV6=1)
