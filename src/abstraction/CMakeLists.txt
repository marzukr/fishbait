cmake_minimum_required(VERSION 3.18.2)
project(abstraction)

include_directories(../hand_evaluator ../omp ../isomorphism)

FIND_PACKAGE(Boost COMPONENTS python38)
FIND_PACKAGE(PythonInterp 3)
FIND_PACKAGE(PythonLibs 3 REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
LINK_LIBRARIES(${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

PYTHON_ADD_MODULE(hand_evaluator cpp_evaluate.cpp)

set_property(TARGET hand_evaluator PROPERTY CXX_STANDARD 17)

find_library(OMP_LIB omp_lib ../omp)
if (${OMP_LIB} STREQUAL "OMP_LIB-NOTFOUND")
  message(FATAL_ERROR "omp not found")
else()
  message(STATUS "omp found as ${OMP_LIB}")
endif()

find_library(ISO_LIB isomorphism_lib ../isomorphism)
if (${ISO_LIB} STREQUAL "ISO_LIB-NOTFOUND")
  message(FATAL_ERROR "iso not found")
else()
  message(STATUS "iso found as ${ISO_LIB}")
endif()

target_link_libraries(hand_evaluator ${OMP_LIB})

add_executable(cpp_experiment.out cpp_experiment.cpp)
target_link_libraries(cpp_experiment.out isomorphism_lib)

