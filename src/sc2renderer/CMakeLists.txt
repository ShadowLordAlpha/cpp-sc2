set(sc2renderer_sources
    sc2_renderer.cc
    sc2_renderer.h
)

add_library(sc2renderer STATIC ${sc2renderer_sources})

set_target_properties(sc2renderer PROPERTIES FOLDER utilities)

target_include_directories(sc2renderer SYSTEM PRIVATE "${sdl_SOURCE_DIR}/include")

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(sc2renderer
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

if (MSVC)
    target_compile_options(sc2renderer PRIVATE /W4 /WX-)
endif()
