set(sc2renderer_sources sc2_renderer.cc)

add_library(sc2renderer STATIC ${sc2renderer_sources})

set_target_properties(sc2renderer PROPERTIES FOLDER utilities)

target_include_directories(sc2renderer PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(sc2renderer SYSTEM PRIVATE "${sdl_SOURCE_DIR}/include")

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(sc2renderer
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
