# tsp_algo_lib/CMakeLists.txt
project(tsp_algo_lib)

set(TSP_ALGO_SOURCES
        src/tsp_instance.c
        src/tsp_solution.c
        src/feasibility_result.c
        src/tsp_math_util.c
)

add_library(tsp_algo_lib ${TSP_ALGO_SOURCES})
target_include_directories(tsp_algo_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(tsp_algo_lib PRIVATE common)
