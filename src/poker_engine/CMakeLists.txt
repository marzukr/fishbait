add_library(
  poker_engine SHARED
  agent.cc
  agent.h
  game.cc
  game.h
  )
set_property(TARGET poker_engine PROPERTY CXX_STANDARD 17)

add_executable(simulator.out main.cc)

target_link_libraries(simulator.out poker_engine)
set_property(TARGET simulator.out PROPERTY CXX_STANDARD 17)

add_custom_target(simulator ALL)
add_dependencies(simulator simulator.out)
