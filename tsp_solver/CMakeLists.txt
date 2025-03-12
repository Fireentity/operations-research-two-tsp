# tsp_solver/CMakeLists.txt
project(tsp_solver)

set(TSP_SOLVER_SOURCES
        src/main.c
        src/algorithm/nearest_neighbor.c
        src/util/plot_util.c
        src/cmd_options.c
)

add_executable(tsp_solver ${TSP_SOLVER_SOURCES})
target_include_directories(tsp_solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/algorithms ${CMAKE_CURRENT_SOURCE_DIR}/include/util)
target_link_libraries(tsp_solver PRIVATE cli_lib tsp_algo_lib common m)
