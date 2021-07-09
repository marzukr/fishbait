add_library(
  blueprint INTERFACE
  )

add_executable(blueprint.out main.cc)
target_link_libraries(blueprint.out blueprint)
coverage_counter_zero(blueprint.out)
