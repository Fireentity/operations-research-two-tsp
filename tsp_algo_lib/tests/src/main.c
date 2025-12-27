#include <stdio.h>
#include <time.h>

#include "em_test.h"
#include "exact_test.h"
#include "genetic_test.h"
#include "grasp_test.h"
#include "vns_test.h"
#include "n_opt_test.h"
#include "grasp_nn_helpers_test.h"
#include "local_search_test.h"
#include "ts_test.h"
#include "nn_test.h"
#include "parser_test.h"
#include "utility_test.h"
#include "random.h"
#include "subtour_separator_test.h"

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("=== TSP Algorithm Tests Start ===\n\n");
    global_random_init(10);

    run_utility_tests();
    run_parser_tests();
    run_local_search_tests();
    run_nn_tests();
    run_grasp_tests();
    run_ts_tests();
    run_vns_tests();
    run_em_tests();
    run_genetic_tests();
    run_n_opt_tests();
    run_grasp_nn_helpers_tests();
    printf("=== TSP Cplex Tests Start ===\n\n");
    run_subtour_separator_tests();
    run_exact_tests();

    printf("\n=== All Tests Passed Successfully ===\n");
    return 0;
}
