add_library(
  poker_engine SHARED
  agent.cc
  agent.h
  game.cc
  game.h
  )

add_executable(poker_demo.out main.cc)

target_link_libraries(poker_demo.out poker_engine)
