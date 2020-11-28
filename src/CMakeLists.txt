cmake_minimum_required(VERSION 3.5.0)
project(blueprint)

add_subdirectory(hand_evaluator)
add_subdirectory(omp)
add_subdirectory(isomorphism)
add_subdirectory(abstraction)

add_library(
    poker SHARED
    Agent.cpp
    Agent.h
    Game.cpp
    Game.h
    )

add_executable(main.out main.cpp)

target_link_libraries(main.out hand_evaluator_lib poker)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")
