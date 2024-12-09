cmake_minimum_required(VERSION 3.20.0)
project(fishbait)

set(CMAKE_CXX_STANDARD 17)

# AWS Graviton2 optimizations
if(TGT_SYSTEM STREQUAL "Graviton2")
  add_compile_options(-march=armv8.2-a+fp16+rcpc+dotprod+crypto
                      -mtune=neoverse-n1)
  message(STATUS "Optimizing for AWS Graviton2")
else()
  add_compile_options(-march=native)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/out)

include_directories(src)
include_directories(external)
include_directories(external/Catch2/single_include)
include_directories(external/cereal/include)
include_directories(external/hdf5/c++/src)
include_directories(external/hdf5/src)
include_directories(${CMAKE_BINARY_DIR}/external/hdf5/src)

#[[ Adds the given path as a subdirectory, and sorts all targets into the
    <executables>, <interfaces>, and <libraries> variables. ]]
function(add_module module_path)
  add_subdirectory(${module_path})
  get_property(targets DIRECTORY ${module_path} PROPERTY BUILDSYSTEM_TARGETS)
  foreach(target ${targets})
    get_target_property(target_type ${target} TYPE)
    if (target_type STREQUAL "EXECUTABLE")
      set(executables ${executables} ${target} PARENT_SCOPE)
    elseif(target_type STREQUAL "INTERFACE_LIBRARY")
      set(interfaces ${interfaces} ${target} PARENT_SCOPE)
    elseif(target_type STREQUAL "STATIC_LIBRARY" OR
           target_type STREQUAL "MODULE_LIBRARY" OR
           target_type STREQUAL "SHARED_LIBRARY" OR
           target_type STREQUAL "OBJECT_LIBRARY")
      set(libraries ${libraries} ${target} PARENT_SCOPE)
    endif()
  endforeach()
endfunction()

add_module(external/hand-isomorphism/src)
set(HDF5_EXTERNALLY_CONFIGURED ON CACHE BOOL "Set hdf5 variables externally"
    FORCE)
set(HDF5_BUILD_CPP_LIB ON CACHE BOOL "Build hdf5 cpp lib" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Don't build hdf5 testing" FORCE)
set(HDF5_INSTALL_BIN_DIR external)
add_module(external/hdf5)

# Helpful warning compiler flags only for our source files
add_compile_options(-Wall -Wextra -pedantic -Werror)

add_module(src/clustering)
add_module(src/hand_strengths)
add_module(src/mccfr)
add_module(src/poker)
add_module(src/relay)
add_module(src/utils)

add_module(tests)
