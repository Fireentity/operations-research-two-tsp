# Top-level CMakeLists.txt
cmake_minimum_required(VERSION 3.16.1)
project(operations-research-two-tsp C)
set(CMAKE_C_STANDARD 99)

enable_testing()

add_subdirectory(cli_lib)
add_subdirectory(cli_lib/tests)
add_subdirectory(tsp_algo_lib)
add_subdirectory(tsp_solver)
add_subdirectory(common)
add_subdirectory(tsp_solver/tests)

