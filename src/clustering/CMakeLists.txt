add_library(
  clustering SHARED
  cluster_table.cc
  cluster_table.h
  definitions.h
  distance.h
  k_means.h
  )
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(clustering engine Threads::Threads)

add_executable(clustering.out main.cc)
target_link_libraries(clustering.out clustering)
