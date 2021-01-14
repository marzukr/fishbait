find_package(Cython REQUIRED)

# cython_add_standalone_executable(distribution_gen MAIN_MODULE "main.pyx")

add_cython_target(distribution_gen distribution_gen.pyx EMBED_MAIN)