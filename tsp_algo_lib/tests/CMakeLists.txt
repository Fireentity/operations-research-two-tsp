cmake_minimum_required(VERSION 3.16)
project(tsp_algo_lib_tests C)

set(TEST_SOURCES
        src/heuristics/grasp_test.c
        src/heuristics/ts_test.c
        src/heuristics/vns_test.c
        src/heuristics/em_test.c
        src/heuristics/genetic_test.c
        src/components/n_opt_test.c
        src/heuristics/nn_test.c
        src/infrastructure/grasp_nn_helpers_test.c
        src/infrastructure/parser_test.c
        src/components/local_search_test.c
        src/infrastructure/utility_test.c
        src/exacts/exact_test.c
        src/components/subtour_separator_test.c
        src/exacts/hard_fixing_test.c
        src/exacts/local_branching_test.c
)

add_executable(tsp_alg_tests src/main.c ${TEST_SOURCES})
target_include_directories(tsp_alg_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(tsp_alg_tests PRIVATE tsp_algo_lib cflaglib common m)
target_compile_options(tsp_alg_tests PRIVATE -UNDEBUG)
add_test(NAME tsp_alg_lib_tests COMMAND $<TARGET_FILE:tsp_alg_tests>)