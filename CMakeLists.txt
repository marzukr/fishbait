cmake_minimum_required(VERSION 3.10.0)
project(fishbait)

set(CMAKE_CXX_STANDARD 17)

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

include_directories(src)
include_directories(external)
include_directories(external/Catch2/single_include)
include_directories(external/cereal/include)

add_subdirectory(external/hand-isomorphism)

add_subdirectory(src/clustering)
add_subdirectory(src/hand_strengths)
add_subdirectory(src/poker_engine)
add_subdirectory(src/sequence_table)

add_subdirectory(tests)
