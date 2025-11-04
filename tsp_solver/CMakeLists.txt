# tsp_solver/CMakeLists.txt
project(tsp_solver)

set(TSP_SOLVER_SOURCES
        include/plot_util.h
        include/algorithm_plotter.h
        src/main.c
        src/plot_util.c
        src/costs_plotter.c
        src/cmd_options.c
        include/algorithm_runner.h
        src/algorithm_runner.c
)

add_executable(tsp_solver ${TSP_SOLVER_SOURCES})
target_include_directories(tsp_solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(tsp_solver PRIVATE cflaglib tsp_algo_lib common m)
