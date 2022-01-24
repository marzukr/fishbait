# Copyright 2021 Marzuk Rashid

add_library(
  relay SHARED
  commander.cc
  commander.h
  )
target_link_libraries(relay blueprint clustering poker utils)
