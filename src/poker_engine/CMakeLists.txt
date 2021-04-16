add_library(
  poker_engine SHARED
  agent.cc
  agent.h
  game.cc
  game.h
  )

add_executable(simulator.out main.cc)

target_link_libraries(simulator.out poker_engine)

add_custom_target(simulator ALL)
add_dependencies(simulator simulator.out)
