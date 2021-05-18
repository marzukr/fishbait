cmake_minimum_required(VERSION 3.20.0)
project(fishbait)

set(CMAKE_CXX_STANDARD 17)

# Helpful warning compiler flags
add_compile_options(-Wall -Wextra -pedantic -Werror)

# Enable coverage compiler flags if in debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-fprofile-arcs -ftest-coverage)
  add_link_options(-fprofile-arcs)
endif()

# DOWNLOAD ALL THE SUBMODULES
option(GIT_SUBMODULE "Check submodules during build" ON)
if(GIT_SUBMODULE)
  find_package(Git QUIET REQUIRED)
  if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    # Update submodules as needed
    message(STATUS "Submodule update")
    execute_process(COMMAND ${GIT_EXECUTABLE} 
                    submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
      message(FATAL_ERROR "git submodule update --init --recursive failed with " 
                          "Error ${GIT_SUBMOD_RESULT}. Please checkout "
                          "submodules.")
    else()
      message(STATUS "Submodule update - done")
    endif()
  endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/out)

include_directories(src)
include_directories(external)
include_directories(external/Catch2/single_include)
include_directories(external/cereal/include)

#[[ Configures such that the target's .gcno and .gcda coverage files are deleted 
    when make clean is run ]]
function(clean_gcov_files target)
  get_target_property(sources ${target} SOURCES)
  get_target_property(build_path ${target} BINARY_DIR)
  foreach(source ${sources})
    if(source MATCHES "((\.c)|(\.cc))$")
      set(target_gcov_files ${target_gcov_files}
          "${build_path}/CMakeFiles/${target}.dir/${source}.gcno"
          "${build_path}/CMakeFiles/${target}.dir/${source}.gcda")
    endif()
  endforeach()
  get_target_property(clean_files ${target} ADDITIONAL_CLEAN_FILES)
  set(clean_files ${clean_files} ${target_gcov_files})
  set_target_properties(${target} PROPERTIES
                        ADDITIONAL_CLEAN_FILES "${clean_files}")
endfunction()

#[[ Adds the given path as a subdirectory, runs clean_gcov_files on all targets
    in that subdirectory, and sorts all targets into the <executables>,
    <interfaces>, and <libraries> variables. ]]
function(add_module module_path)
  add_subdirectory(${module_path})
  get_property(targets DIRECTORY ${module_path} PROPERTY BUILDSYSTEM_TARGETS)
  foreach(target ${targets})
    get_target_property(target_type ${target} TYPE)
    if (target_type STREQUAL "EXECUTABLE")
      set(executables ${executables} ${target} PARENT_SCOPE)
      clean_gcov_files(${target})
    elseif(target_type STREQUAL "INTERFACE_LIBRARY")
      set(interfaces ${interfaces} ${target} PARENT_SCOPE)
    else()
      set(libraries ${libraries} ${target} PARENT_SCOPE)
      clean_gcov_files(${target})
    endif()
  endforeach()
endfunction()

add_module(external/hand-isomorphism/src)

add_module(src/blueprint)
add_module(src/clustering)
add_module(src/deck)
add_module(src/hand_strengths)
add_module(src/poker_engine)
add_module(src/utils)

add_module(tests)

# Create the coverage target if we are in debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_custom_target(coverage
    COMMAND lcov --zerocounters --directory .
    COMMAND lcov --capture --no-external --initial --directory .
            --base-directory ${CMAKE_SOURCE_DIR}/src
            --output-file tests/base_coverage.info
    COMMAND ./bin/tests.out
    COMMAND lcov --capture --no-external --directory .
            --base-directory ${CMAKE_SOURCE_DIR}/src
            --output-file tests/test_coverage.info
    COMMAND lcov --add-tracefile tests/base_coverage.info
            --add-tracefile tests/test_coverage.info
            --output-file tests/total_coverage.info
    COMMAND genhtml tests/total_coverage.info
            --output-directory tests/lcov_html
    COMMAND lcov --zerocounters --directory .
    COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated at:"
            ${CMAKE_BINARY_DIR}/tests/lcov_html/index.html
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    BYPRODUCTS ${CMAKE_BINARY_DIR}/tests/lcov_html/
               ${CMAKE_BINARY_DIR}/tests/base_coverage.info
               ${CMAKE_BINARY_DIR}/tests/test_coverage.info
               ${CMAKE_BINARY_DIR}/tests/total_coverage.info
    )
  add_dependencies(coverage ${interfaces} ${libraries} ${executables})
endif()
