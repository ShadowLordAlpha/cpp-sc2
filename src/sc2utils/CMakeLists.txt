set(sc2utils_sources
        sc2_search.cc
        sc2_utils.cc
        arg_parser.cpp
        platform.cpp
)

add_library(sc2utils STATIC ${sc2utils_sources})

target_include_directories(sc2utils PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(sc2utils PRIVATE spdlog::spdlog sc2protocol)
