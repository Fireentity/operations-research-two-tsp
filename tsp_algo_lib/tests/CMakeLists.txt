cmake_minimum_required(VERSION 3.16)
project(tsp_algo_lib_tests C)

set(TEST_SOURCES
        src/grasp_test.c
        src/ts_test.c
        src/vns_test.c
        src/em_test.c
        src/genetic_test.c
        src/n_opt_test.c
        src/nn_test.c
        src/grasp_nn_helpers_test.c
        src/parser_test.c
        src/local_search_test.c
        src/utility_test.c
)

add_executable(tsp_alg_tests src/main.c ${TEST_SOURCES})

target_include_directories(tsp_alg_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(tsp_alg_tests PRIVATE tsp_algo_lib cflaglib common m)

add_test(NAME tsp_alg_lib_tests COMMAND $<TARGET_FILE:tsp_alg_tests>)