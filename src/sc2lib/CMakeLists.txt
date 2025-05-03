set(sc2lib_sources
    sc2_search.cc
    sc2_utils.cc)

add_library(sc2lib STATIC ${sc2lib_sources})

target_link_libraries(sc2lib PRIVATE sc2api)
