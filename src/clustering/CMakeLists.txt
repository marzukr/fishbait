add_executable(clustering.out main.cc)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(clustering.out PRIVATE Threads::Threads)

add_custom_target(clustering ALL)
add_dependencies(clustering clustering.out)
