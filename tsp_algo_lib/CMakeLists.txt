project(tsp_algo_lib)

# Set compile options for tsp_algo_lib
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Define the sources for tsp_algo_lib
set(TSP_ALGO_SOURCES
        ../tsp_solver/src/plot_util.c
        ../tsp_solver/src/costs_plotter.c
        src/algorithm/grasp.c
        src/algorithm/nearest_neighbor.c
        src/algorithm/tabu_search.c
        src/algorithm/variable_neighborhood_search.c
        src/api/algorithms.c
        src/api/tsp_instance.c
        src/api/tsp_solution.c
        src/api/feasibility_result.c
        src/api/tsp_math_util.c
        src/api/time_limiter.c
)

# Create a library for tsp_algo_lib
add_library(tsp_algo_lib ${TSP_ALGO_SOURCES})

# Include directories for tsp_algo_lib
target_include_directories(tsp_algo_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/ ${CMAKE_CURRENT_SOURCE_DIR}/include/plotting ${CMAKE_CURRENT_SOURCE_DIR}/include/algorithms ${CMAKE_CURRENT_SOURCE_DIR}/include/api)

# Link tsp_algo_lib with common
target_link_libraries(tsp_algo_lib PRIVATE common)
