# tsp_solver/CMakeLists.txt
project(tsp_solver)

set(TSP_SOLVER_SOURCES
        src/main.c
        src/plotting/plot_util.c
        src/plotting/costs_plotter.c
        src/cmd_option/cmd_options.c
        src/cmd_option/cmd_option_base.c
        src/cmd_option/cmd_option_cli.c
        src/cmd_option/cmd_option_ini.c
        include/algorithm_runner.h
        src/algorithm_runner.c
)

add_executable(tsp_solver ${TSP_SOLVER_SOURCES})
target_include_directories(tsp_solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/cmd_option ${CMAKE_CURRENT_SOURCE_DIR}/include/plotting)
target_link_libraries(tsp_solver PRIVATE cflaglib tsp_algo_lib common m)
