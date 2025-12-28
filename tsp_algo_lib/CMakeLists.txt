project(tsp_algo_lib)

set(TSP_EXACT_SOURCES
        src/algorithm/exact/cplex_solver_wrapper.c
        src/algorithm/exact/subtour_separator.c
        src/algorithm/benders_loop.c
        src/algorithm/branch_and_cut.c
        src/algorithm/hard_fixing.c
)

set(TSP_ALGO_SOURCES
        src/algorithm/exact/cplex_solver_wrapper.c
        src/algorithm/exact/subtour_separator.c
        src/algorithm/benders_loop.c
        src/algorithm/branch_and_cut.c
        src/algorithm/grasp.c
        src/algorithm/nearest_neighbor.c
        src/algorithm/tabu_search.c
        src/algorithm/variable_neighborhood_search.c
        src/algorithm/extra_mileage.c
        src/algorithm/genetic.c
        src/algorithm/heuristic/constructive.c
        src/algorithm/heuristic/local_search.c
        src/api/tsp_instance.c
        src/api/tsp_solution.c
        src/api/tsp_algorithm.c
        src/utility/tsp_math.c
        src/utility/tsp_tour.c
        src/utility/tsp_error.c
        src/utility/feasibility_result.c
        src/utility/time_limiter.c
        src/utility/cost_recorder.c
        src/parser/tsp_parser.c
        src/parser/instance/tsp_parser_tsplib.c
        src/parser/solution/tsp_parser_sol_v1.c
        ${TSP_EXACT_SOURCES}
)

add_library(tsp_algo_lib ${TSP_ALGO_SOURCES})

target_include_directories(tsp_algo_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/algorithms
        ${CMAKE_CURRENT_SOURCE_DIR}/include/algorithms/heuristic
        ${CMAKE_CURRENT_SOURCE_DIR}/include/algorithms/exact
        ${CMAKE_CURRENT_SOURCE_DIR}/include/api
        ${CMAKE_CURRENT_SOURCE_DIR}/include/parser
        ${CMAKE_CURRENT_SOURCE_DIR}/include/parser/solution
        ${CMAKE_CURRENT_SOURCE_DIR}/include/parser/instance
        ${CMAKE_CURRENT_SOURCE_DIR}/include/utility
)

target_link_libraries(tsp_algo_lib PRIVATE common)

if(CPLEX_FOUND)
    target_link_libraries(tsp_algo_lib PRIVATE CPLEX::CPLEX)
endif()