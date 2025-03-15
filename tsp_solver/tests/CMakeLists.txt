cmake_minimum_required(VERSION 3.16)
project(tsp_solver_tests C)

add_executable(tsp_solver_tests n_opt_test.c
)
target_link_libraries(tsp_solver_tests PRIVATE m)

add_test(NAME tsp_solver_tests COMMAND $<TARGET_FILE:tsp_solver_tests>)
