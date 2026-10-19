# Apply the civetweb patches to the fetched source, whatever state an earlier run left it in.
#
# FetchContent runs the patch step again whenever the step's stamp is out of date, for example when a build directory
# is reused with a different source tree (a CI cache, or a Docker build volume shared by several checkouts) and the
# sub-build is regenerated. UPDATE_DISCONNECTED keeps the source as it was, already patched, so a plain `git apply`
# then fails with "patch does not apply" and the configure stops.
#
# So, in order:
#   1. If every patch un-applies cleanly (`git apply --reverse --check`), the patches are in: nothing is done, and no
#      file is touched.
#   2. Otherwise, when the source is a git checkout (FetchContent's GIT_REPOSITORY), the files the patches touch are
#      restored from HEAD first. This covers a source patched earlier from copies of the patches with other line
#      endings (a Windows checkout with core.autocrlf and a Linux one sharing a build directory): `--ignore-whitespace`
#      relaxes only context lines, so neither the reverse check nor a second apply matches it. Local edits to those
#      files are discarded; they belong to the fetched dependency, not to this project.
#   3. The patches are applied. A failure here stops the configure.
#
# Usage, from the source directory (FetchContent runs PATCH_COMMAND there):
#   cmake -DGIT_EXECUTABLE=<git> -DPATCH_DIR=<dir> -P apply-patches.cmake
# Every *.patch file in PATCH_DIR is applied, in file name order. GIT_EXECUTABLE is optional.

if(NOT PATCH_DIR)
    message(FATAL_ERROR "apply-patches.cmake: PATCH_DIR is not set")
endif()

if(NOT GIT_EXECUTABLE)
    find_program(GIT_EXECUTABLE git)
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "apply-patches.cmake: git not found")
    endif()
endif()

file(GLOB patches "${PATCH_DIR}/*.patch")
list(SORT patches)
if(NOT patches)
    message(FATAL_ERROR "apply-patches.cmake: no *.patch file in ${PATCH_DIR}")
endif()

# 1. Already applied?
execute_process(
    COMMAND "${GIT_EXECUTABLE}" apply --reverse --check --ignore-whitespace ${patches}
    RESULT_VARIABLE reverse_result
    OUTPUT_QUIET
    ERROR_QUIET
)
if(reverse_result EQUAL 0)
    message(STATUS "civetweb: patches already applied")
    return()
endif()

# 2. Restore the files the patches touch, when this is a git checkout. A file a patch creates is removed instead.
execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse --is-inside-work-tree
    RESULT_VARIABLE inside_result
    OUTPUT_QUIET
    ERROR_QUIET
)
if(inside_result EQUAL 0)
    set(touched "")
    set(created "")
    foreach(patch IN LISTS patches)
        file(STRINGS "${patch}" lines REGEX "^(---|\\+\\+\\+) ")
        set(old_path "")
        foreach(line IN LISTS lines)
            string(REGEX REPLACE "[ \t\r]+$" "" line "${line}")
            if(line MATCHES "^--- (.*)$")
                set(old_path "${CMAKE_MATCH_1}")
            elseif(line MATCHES "^\\+\\+\\+ b/(.*)$")
                if(old_path STREQUAL "/dev/null")
                    list(APPEND created "${CMAKE_MATCH_1}")
                else()
                    list(APPEND touched "${CMAKE_MATCH_1}")
                endif()
            endif()
        endforeach()
    endforeach()
    list(REMOVE_DUPLICATES touched)
    if(touched)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" checkout -- ${touched}
            RESULT_VARIABLE checkout_result
        )
        if(NOT checkout_result EQUAL 0)
            message(FATAL_ERROR "apply-patches.cmake: git checkout of ${touched} failed (${checkout_result})")
        endif()
    endif()
    foreach(path IN LISTS created)
        file(REMOVE "${path}")
    endforeach()
    message(STATUS "civetweb: restored ${touched} before patching")
endif()

# 3. Apply.
execute_process(
    COMMAND "${GIT_EXECUTABLE}" apply --ignore-whitespace ${patches}
    RESULT_VARIABLE apply_result
)
if(NOT apply_result EQUAL 0)
    message(FATAL_ERROR "apply-patches.cmake: git apply failed (${apply_result})")
endif()
