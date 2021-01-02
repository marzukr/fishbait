cmake_minimum_required(VERSION 3.5.0)
project(fishbait)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src)
include_directories(external)

add_subdirectory(src/clustering)
add_subdirectory(external/OMPEval)
add_subdirectory(external/hand-isomorphism)