cmake_minimum_required(VERSION 3.5.0)
project(fishbait_pokerengine)

add_library(
  poker_engine SHARED
  Agent.cc
  Agent.h
  Game.cc
  Game.h
  )
set_property(TARGET poker_engine PROPERTY CXX_STANDARD 17)

add_executable(simulator.out main.cc)

target_link_libraries(simulator.out poker_engine)
set_property(TARGET simulator.out PROPERTY CXX_STANDARD 17)

add_custom_target(simulator)
add_dependencies(simulator simulator.out)