# ----------------------------------------------------------------------------
# Build logic for examples of the cppsc2 library (subdirectory)
#
# This CMake file:
#   - Provides add_example function
#   - Compiles all example projects
#
# Requirements:
#   * CMake >= 3.14 (for protobuf_generate_cpp TARGET argument)
# ----------------------------------------------------------------------------

# -------------------------------
# add_example method
#
# target_* methods provided by cmake can be used same as add_executable
# target_include_directories(${example_name} PRIVATE path/to/include)
# target_link_libraries(${example_name} PRIVATE common_lib)
# -------------------------------
function(add_example example_name)
    # Collect all arguments after the first one as sources
    set(_sources ${ARGN})

    message(VERBOSE "Adding example project: ${example_name}")
    # Create the executable like add_executable does
    add_executable(${example_name} ${_sources})

    # Group examples under 'examples' folder in IDEs like CLion/VS
    set_target_properties(${example_name} PROPERTIES FOLDER examples)
endfunction()

add_example(tutorial1 tutorial/tutorial1.cpp)
target_link_libraries(tutorial1 PRIVATE cpp_sc2)

add_example(tutorial2 tutorial/tutorial2.cpp)
target_link_libraries(tutorial2 PRIVATE cpp_sc2)

add_example(tutorial3 tutorial/tutorial3.cpp)
target_link_libraries(tutorial3 PRIVATE cpp_sc2)

# TODO: old data, need to redo some of this

# Function to generate an example project with extra libraries
#function (example_project_extra project_name source_code extra_libs)
#    message(VERBOSE "Adding new example project ${project_name},\
#            sources: ${source_code}, extra_libs: ${extra_libs}")
#    set(example_project_sources
#        ${source_code}
#        common/bot_examples.cc)
#
#    add_executable(${project_name} ${example_project_sources})
#
#    target_include_directories(${project_name} PRIVATE common)
#
#    set_target_properties(${project_name} PROPERTIES FOLDER examples)
#
#    target_link_libraries(${project_name} PRIVATE cpp_sc2 ${extra_libs})
#endfunction ()
#
## Function to generate a simple example project
#function (example_project project_name source_code)
#    example_project_extra(${project_name} ${source_code} "")
#endfunction ()
#
#example_project(bot_simple bot_simple.cc)
#example_project(echo_actions echo_actions.cc)
#example_project(replay replay.cc)
#example_project(tutorial tutorial.cc)
#example_project(bot_micro bot_micro.cc)
#example_project(bot_mp bot_mp.cc)
#example_project(intermediate_bot intermediate_bot.cc)
#example_project(annoying_helper annoying_helper.cc)
#example_project(proxy proxy.cc)
#example_project(save_load save_load.cc)
#
#if (BUILD_SC2_RENDERER)
#    example_project_extra(feature_layers feature_layers.cc sc2renderer)
#    example_project_extra(rendered rendered.cc sc2renderer)
#endif ()
