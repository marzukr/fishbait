add_library(
  hand_strengths SHARED
  lut_generators.cc
  lut_generators.h
  ochs.cc
  ochs.h
  )
target_link_libraries(hand_strengths handisomorphism deck)

add_executable(hand_strengths.out main.cc)
target_link_libraries(hand_strengths.out hand_strengths deck)

add_custom_target(hand_strengths_directory ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory
          ${CMAKE_BINARY_DIR}/out/hand_strengths
  )
set_target_properties(hand_strengths.out PROPERTIES ADDITIONAL_CLEAN_FILES
                      ${CMAKE_BINARY_DIR}/out/hand_strengths/)
add_dependencies(hand_strengths.out hand_strengths_directory)
