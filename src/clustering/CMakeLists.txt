add_executable(clustering.out main.cc)

add_custom_target(clustering ALL)
add_dependencies(clustering clustering.out)
