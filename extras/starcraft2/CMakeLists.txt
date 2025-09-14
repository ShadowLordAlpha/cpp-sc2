add_subdirectory(external)

set(ASTRA_SC2_SRCS
        src/sc2/sc2_game.cpp
        src/sc2/api/sc2_api.cpp
)

add_library(starcraft2 STATIC ${ASTRA_SC2_SRCS})
target_include_directories(starcraft2
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
)

# Link deps:
# - Make a dependency PUBLIC only if types/macros from it appear in public headers under include/
# - Otherwise keep it PRIVATE.
target_link_libraries(starcraft2
    PUBLIC
        glm::glm                # keep PUBLIC only if public headers use glm types
        astra::astra
    PRIVATE
        astra::sc2proto         # keep PUBLIC only if public headers expose proto types/enums
        spdlog::spdlog
)

# Namespace alias for consumers
add_library(astra::sc2api ALIAS starcraft2)
