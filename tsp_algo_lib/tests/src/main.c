#include <stdio.h>
#include "grasp_test.h"
#include "vns_test.h"
#include "n_opt_test.h"
#include "grasp_nn_helpers_test.h"
#include "ts_test.h"
#include "nn_test.h"

int main(void) {
    printf("=== Running All TSP Algorithm Tests ===\n\n");

    // Test degli algoritmi principali
    run_grasp_tests();
    run_ts_tests();
    run_vns_tests();
    run_nn_tests();
    run_n_opt_tests();
    run_grasp_nn_helpers_tests();

    printf("\n=== All Tests Passed ===\n");
    return 0;
} //TODO fix tests
