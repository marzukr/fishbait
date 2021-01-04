add_executable(tests.out main.cc clustering/ArrayTest.cc)
set_property(TARGET tests.out PROPERTY CXX_STANDARD 17)

add_custom_target(tests)
add_dependencies(tests tests.out)
