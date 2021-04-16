add_executable(clustering.out main.cc)
set_property(TARGET clustering.out PROPERTY CXX_STANDARD 17)

add_custom_target(clustering ALL)
add_dependencies(clustering clustering.out)
