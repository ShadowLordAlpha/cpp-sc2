add_library(sc2api STATIC temp.cpp)
target_link_libraries(sc2api PRIVATE cpp-sc2::sc2proto)
