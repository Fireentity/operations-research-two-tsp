# tsp_solver/CMakeLists.txt
project(tsp_solver)

set(TSP_SOLVER_SOURCES
        src/main.c
)

add_executable(tsp_solver ${TSP_SOLVER_SOURCES})
target_include_directories(tsp_solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(tsp_solver PRIVATE cli_lib tsp_algo_lib)
