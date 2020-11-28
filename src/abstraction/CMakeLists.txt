cmake_minimum_required(VERSION 3.5.0)
project(abstraction)

if(NOT MACHINE STREQUAL "Mac" AND NOT MACHINE STREQUAL "EWS")
  message(FATAL_ERROR "MACHINE is \"${MACHINE}\". It must be \"Mac\" or \"EWS\"")
endif()

include_directories(../hand_evaluator ../omp ../isomorphism)

if(MACHINE EQUAL "Mac")
  FIND_PACKAGE(Boost COMPONENTS python38)
endif()

if(MACHINE EQUAL "EWS")
  set(Boost_INCLUDE_DIR /home/marzukr2/boost_1_59_0)
  set(Boost_LIBRARY_DIR /home/marzukr2/boost_1_59_0/stage/lib)
  find_package(Boost 1.59.0 REQUIRED COMPONENTS python3)
endif()

FIND_PACKAGE(PythonInterp 3)
FIND_PACKAGE(PythonLibs 3 REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
LINK_LIBRARIES(${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

PYTHON_ADD_MODULE(hand_evaluator cpp_evaluate.cpp)

set_property(TARGET hand_evaluator PROPERTY CXX_STANDARD 14)

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

if(MACHINE EQUAL "Mac")
  target_link_libraries(hand_evaluator ${OMP_LIB} isomorphism_lib)
endif()

if(MACHINE EQUAL "EWS")
  target_link_libraries(hand_evaluator /home/marzukr2/poker-bot/src/omp/libomp_lib.so /home/marzukr2/poker-bot/src/isomorphism/libisomorphism_lib.so)
  set_target_properties(hand_evaluator PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

add_executable(cpp_experiment.out cpp_experiment.cpp)

if(MACHINE EQUAL "Mac")
  target_link_libraries(cpp_experiment.out isomorphism_lib)
endif()

if(MACHINE EQUAL "EWS")
  target_link_libraries(cpp_experiment.out /home/marzukr2/poker-bot/src/isomorphism/libisomorphism_lib.so)
endif()

