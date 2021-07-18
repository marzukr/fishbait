add_library(
  clustering INTERFACE
  )
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(clustering INTERFACE Threads::Threads)

add_executable(clustering.out main.cc)
target_link_libraries(clustering.out clustering)
