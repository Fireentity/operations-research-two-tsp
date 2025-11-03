cmake_minimum_required(VERSION 3.16.1)
project(operations-research-two-tsp C)
set(CMAKE_C_STANDARD 99)

# Enable testing
enable_testing()

# Add subdirectories for each module
add_subdirectory(cflaglib)
add_subdirectory(cflaglib/tests)
add_subdirectory(cli_lib)
add_subdirectory(cli_lib/tests)
add_subdirectory(tsp_algo_lib)
add_subdirectory(tsp_algo_lib/tests)
add_subdirectory(tsp_solver)
add_subdirectory(common)
add_subdirectory(common/tests)
