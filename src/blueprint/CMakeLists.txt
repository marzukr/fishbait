add_library(
  blueprint INTERFACE
  )

add_executable(blueprint.out main.cc ${CMAKE_BINARY_DIR}/out/blueprint)
target_link_libraries(blueprint.out blueprint clustering)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/out/blueprint
  COMMAND ${CMAKE_COMMAND} -E make_directory
          ${CMAKE_BINARY_DIR}/out/blueprint
  )
