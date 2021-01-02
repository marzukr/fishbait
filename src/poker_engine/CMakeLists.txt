cmake_minimum_required(VERSION 3.5.0)
project(fishbait_pokerengine)

add_library(
  poker_engine SHARED
  Agent.cc
  Agent.h
  Game.cc
  Game.h
  )

add_executable(simulator.out main.cc)

target_link_libraries(simulator.out poker_engine)
