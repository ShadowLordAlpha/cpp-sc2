set(sc2utils_sources
        sc2_utils.cc
        arg_parser.cpp
        platform.cpp
)

add_library(sc2utils STATIC ${sc2utils_sources})

if(WIN32)
    target_sources(sc2utils PRIVATE process_windows.cpp)
else()
    target_sources(sc2utils PRIVATE process_posix.cpp)
endif()

target_include_directories(sc2utils PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(sc2utils PUBLIC spdlog::spdlog sc2api)
