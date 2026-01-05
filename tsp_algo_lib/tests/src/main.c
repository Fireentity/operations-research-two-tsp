#include <stdio.h>

#include "logger.h"
#include "test_instances.h"
#include "random.h"

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("=== TSP Algorithm Tests Start ===\n\n");
    global_random_init(10);
#ifndef DISABLE_VERBOSE
    logger_set_verbosity(VERBOSE_INFO);
#endif

    // Infrastructure & Utilities
    printf("\n--- Infrastructure Tests ---\n");
    run_utility_tests();
    run_parser_tests();
    run_grasp_nn_helpers_tests();

    // Core Components
    printf("\n--- Core Logic Tests ---\n");
    run_local_search_tests();
    run_n_opt_tests();
    run_subtour_separator_tests();

    // Heuristics
    printf("\n--- Heuristic Tests ---\n");
    run_nn_tests();
    run_em_tests();
    run_vns_tests();
    run_ts_tests();
    run_grasp_tests();
    run_genetic_tests();

    // Exact & Matheuristics
    printf("\n--- Exact & Hybrid Tests ---\n");
    run_exact_tests();
    run_hard_fixing_tests();
    run_local_branching_tests();

    printf("\n=== All Tests Passed Successfully ===\n");
    return 0;
}