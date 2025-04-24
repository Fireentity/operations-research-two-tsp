cmake_minimum_required(VERSION 3.16)
project(tsp_algo_lib_tests C)

# Define test source files and executables
add_executable(grasp_nearest_neighbor_test grasp_nearest_neighbor_test.c)
add_executable(n_opt_test n_opt_test.c)
add_executable(nearest_neighbor_test nearest_neighbor_test.c)
add_executable(grasp_test grasp_test.c)

# Link the test executables with required libraries
target_link_libraries(grasp_nearest_neighbor_test PRIVATE tsp_algo_lib cli_lib common m)
target_link_libraries(n_opt_test PRIVATE tsp_algo_lib cli_lib common m)
target_link_libraries(nearest_neighbor_test PRIVATE tsp_algo_lib cli_lib common m)
target_link_libraries(grasp_test PRIVATE tsp_algo_lib cli_lib common m)

# Add individual tests to CTest
add_test(NAME grasp_nearest_neighbor_test COMMAND $<TARGET_FILE:grasp_nearest_neighbor_test>)
add_test(NAME n_opt_test COMMAND $<TARGET_FILE:n_opt_test>)
add_test(NAME nearest_neighbor_test COMMAND $<TARGET_FILE:nearest_neighbor_test>)
add_test(NAME grasp_test COMMAND $<TARGET_FILE:grasp_test>)
