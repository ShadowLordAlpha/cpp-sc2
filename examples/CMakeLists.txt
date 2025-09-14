# ==============================================================================
# examples - Sample code for how to use the library
# ==============================================================================

# Convince method for adding example executables
function(add_example example_name)
    # Collect all arguments after the first one as sources
    set(_sources ${ARGN})

    message(VERBOSE "Adding example project: ${example_name}")
    # Create the executable like add_executable does
    add_executable(${example_name} ${_sources})

    # Group examples under 'examples' folder in IDEs like CLion/VS
    set_target_properties(${example_name} PROPERTIES FOLDER examples)
endfunction()

# Tutorial type examples
add_example(tutorial1 tutorial/tutorial1.cpp)
target_link_libraries(tutorial1 PRIVATE astra::astra)

add_example(tutorial2 tutorial/tutorial2.cpp)
target_link_libraries(tutorial2 PRIVATE astra::astra)

add_example(tutorial3 tutorial/tutorial3.cpp)
target_link_libraries(tutorial3 PRIVATE astra::astra)

add_example(ReplayObserver other/replay_observer.cpp)
target_link_libraries(ReplayObserver PRIVATE astra::astra astra::sc2api)
