cmake_minimum_required(VERSION 3.16)
project(tsp_solver_tests C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  ")

enable_testing()

add_executable(tsp_solver_n_opt_test n_opt_test.c)
target_link_libraries(tsp_solver_n_opt_test PRIVATE tsp_algo_lib cli_lib common m)
add_test(NAME tsp_solver_n_opt_test COMMAND $<TARGET_FILE:tsp_solver_n_opt_test>)

add_executable(tsp_solver_nearest_neighbor_test nearest_neighbor.c)
target_link_libraries(tsp_solver_nearest_neighbor_test PRIVATE tsp_algo_lib cli_lib common m)
add_test(NAME tsp_solver_nearest_neighbor_test COMMAND $<TARGET_FILE:tsp_solver_nearest_neighbor_test>)
