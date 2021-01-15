find_package(mongocxx REQUIRED)

add_executable(clustering.out main.cc)
set_property(TARGET clustering.out PROPERTY CXX_STANDARD 17)
target_link_libraries(clustering.out PRIVATE mongo::mongocxx_shared)

add_custom_target(clustering ALL)
add_dependencies(clustering clustering.out)
