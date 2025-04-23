project(tsp_algo_lib)

# Set compile options for tsp_algo_lib
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Define the sources for tsp_algo_lib
set(TSP_ALGO_SOURCES
        src/algorithms.c
        src/tsp_instance.c
        src/tsp_solution.c
        src/feasibility_result.c
        src/tsp_math_util.c
        src/time_limiter.c
)

# Create a library for tsp_algo_lib
add_library(tsp_algo_lib ${TSP_ALGO_SOURCES})

# Include directories for tsp_algo_lib
target_include_directories(tsp_algo_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link tsp_algo_lib with common
target_link_libraries(tsp_algo_lib PRIVATE common)
