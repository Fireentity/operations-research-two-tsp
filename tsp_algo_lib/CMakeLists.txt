project(tsp_algo_lib)

# Set compile options for tsp_algo_lib
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Define the sources for tsp_algo_lib
set(TSP_ALGO_SOURCES
        src/algorithm/grasp.c
        src/algorithm/nearest_neighbor.c
        src/algorithm/tabu_search.c
        src/algorithm/variable_neighborhood_search.c
        src/algorithm/algorithms.c
        src/api/tsp_instance.c
        src/api/tsp_solution.c
        src/utility/feasibility_result.c
        src/api/tsp_math_util.c
        src/utility/time_limiter.c
        src/parser/tsp_parser.c
        src/parser/tsp_parser_tsplib.c
        src/utility/cost_recorder.c
)

# Create a library for tsp_algo_lib
add_library(tsp_algo_lib ${TSP_ALGO_SOURCES})

# Include directories for tsp_algo_lib
target_include_directories(tsp_algo_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/ ../tsp_solver/include/plotting ${CMAKE_CURRENT_SOURCE_DIR}/include/algorithms ${CMAKE_CURRENT_SOURCE_DIR}/include/api ${CMAKE_CURRENT_SOURCE_DIR}/include/parser ${CMAKE_CURRENT_SOURCE_DIR}/include/utility)

# Link tsp_algo_lib with common
target_link_libraries(tsp_algo_lib PRIVATE common)
