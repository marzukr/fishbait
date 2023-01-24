add_library(
  relay SHARED
  commander.cc
  commander.h
  scribe.h
  )
target_link_libraries(relay blueprint clustering poker utils hdf5_cpp-shared)

add_executable(avg_to_hd5.out avg_to_hd5.cc)
target_link_libraries(avg_to_hd5.out relay)
