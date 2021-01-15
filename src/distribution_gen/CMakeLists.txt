find_package(PythonInterp 3 REQUIRED)
find_package(PythonLibs 3 REQUIRED)

find_package(Cython REQUIRED)
find_package(PythonExtensions REQUIRED)
include(UsePythonExtensions)

add_python_extension(OCHS SOURCES OCHS.pyx)

add_cython_target(distribution_gen distribution_gen.pyx EMBED_MAIN)
add_executable(.distribution_gen.out ${distribution_gen})
python_standalone_executable(.distribution_gen.out)

target_link_libraries(.distribution_gen.out handisomorphism)

add_custom_command(TARGET .distribution_gen.out POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "\"#\!/bin/sh\"" > distribution_gen.sh
  COMMAND ${CMAKE_COMMAND} -E echo "PYTHONPATH=\\\$\$\{PYTHONPATH\}:${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/.distribution_gen.out \$\$\\@" >> distribution_gen.sh
  COMMAND chmod +x distribution_gen.sh
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  DEPENDS .distribution_gen.out
  COMMENT "Writing wrapper script for distribution_gen"
)

add_custom_target(distribution_gen ALL)
add_dependencies(distribution_gen .distribution_gen.out OCHS handisomorphism)
