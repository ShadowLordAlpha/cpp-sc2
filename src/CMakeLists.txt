add_subdirectory(sc2api)
add_subdirectory(sc2utils)
add_subdirectory(sc2protocol)

add_library(cpp_sc2 INTERFACE)

target_link_libraries(cpp_sc2
    INTERFACE
        sc2api
        sc2utils)
