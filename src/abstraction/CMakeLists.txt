cmake_minimum_required(VERSION 3.5.0)
project(abstraction)

if(NOT MACHINE STREQUAL "Mac" AND NOT MACHINE STREQUAL "EWS")
  message(FATAL_ERROR "MACHINE is \"${MACHINE}\". It must be \"Mac\" or \"EWS\"")
endif()

include_directories(../hand_evaluator ../omp ../isomorphism)

if(MACHINE STREQUAL "Mac")
  find_package(Boost COMPONENTS python3)
endif()

if(MACHINE STREQUAL "EWS")
  find_package(Boost 1.70.0 REQUIRED COMPONENTS python)
endif()

find_package(PythonInterp 3)
find_package(PythonLibs 3 REQUIRED)
find_package(mongocxx REQUIRED)

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

add_library(hand_evaluator MODULE cpp_evaluate.cpp)
set_target_properties(hand_evaluator PROPERTIES PREFIX "")
set_property(TARGET hand_evaluator PROPERTY CXX_STANDARD 14)
target_include_directories(hand_evaluator PRIVATE 
  ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
)
target_link_libraries(hand_evaluator 
  ${OMP_LIB} ${ISO_LIB} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES}
)

add_executable(cpp_experiment.out cpp_experiment.cpp)
target_include_directories(cpp_experiment.out PRIVATE
  ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
)
target_link_libraries(cpp_experiment.out 
  ${ISO_LIB} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES}
)

add_executable(cluster.out cluster.cpp)
set_property(TARGET cluster.out PROPERTY CXX_STANDARD 17)
target_link_libraries(cluster.out PRIVATE mongo::mongocxx_shared)
