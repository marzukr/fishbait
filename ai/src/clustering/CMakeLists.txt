add_library(
  clustering SHARED
  cluster_table.cc
  cluster_table.h
  definitions.h
  distance.h
  k_means.h
  matchmaker.h
  test_clusters.h
  )
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(clustering poker Threads::Threads)

add_executable(clustering.out main.cc ${CMAKE_BINARY_DIR}/out/ai/clustering)
target_link_libraries(clustering.out clustering)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/out/ai/clustering
  COMMAND ${CMAKE_COMMAND} -E make_directory
          ${CMAKE_BINARY_DIR}/out/ai/clustering
  )
