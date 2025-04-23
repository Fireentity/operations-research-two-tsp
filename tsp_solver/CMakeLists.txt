project(tsp_solver)

# Set compile options for tsp_solver
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Define the sources for tsp_solver
set(TSP_SOLVER_SOURCES
        src/main.c
        src/algorithm/grasp.c
        src/algorithm/nearest_neighbor.c
        src/algorithm/tabu_search.c
        src/algorithm/variable_neighborhood_search.c
        src/cmd_options.c
        src/util/costs_plotter.c
        src/util/plot_util.c
        src/util/costs_plotter.c
        src/algorithm/tabu_search.c
        src/algorithm/grasp.c
)

# Create an executable for tsp_solver
add_executable(tsp_solver ${TSP_SOLVER_SOURCES})
target_include_directories(tsp_solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/algorithms ${CMAKE_CURRENT_SOURCE_DIR}/include/util)
target_link_libraries(tsp_solver PRIVATE cli_lib tsp_algo_lib common m)
