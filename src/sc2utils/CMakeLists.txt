set(sc2utils_sources
        arg_parser.cpp
        sc2_manage_process.cc
        sc2_scan_directory.cc)

add_library(sc2utils STATIC ${sc2utils_sources})

target_include_directories(sc2utils PRIVATE "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(sc2utils PRIVATE spdlog::spdlog)
