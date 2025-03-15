cmake_minimum_required(VERSION 3.16)
project(tsp_solver_tests C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  ")

enable_testing()

add_executable(tsp_solver_tests n_opt_test.c)
target_link_libraries(tsp_solver_tests PRIVATE tsp_algo_lib cli_lib common m)

add_test(NAME tsp_solver_tests COMMAND $<TARGET_FILE:tsp_solver_tests>)
