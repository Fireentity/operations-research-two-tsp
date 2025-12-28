#include "hard_fixing_test.h"
#include "hard_fixing.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON_TEST 1e-4

void test_hard_fixing_burma14(void) {
    printf("  [HardFixing] Testing burma14 (Fix=0.8, NN warm-start)...\n");

    Node nodes[14] = {
        {16.47, 96.10}, {16.47, 94.44}, {20.09, 92.54}, {22.39, 93.37},
        {25.23, 97.24}, {22.00, 96.05}, {20.47, 97.02}, {17.20, 96.29},
        {16.30, 97.38}, {14.05, 98.12}, {16.53, 97.38}, {21.52, 95.59},
        {19.41, 97.13}, {20.09, 94.55}
    };

    TspInstance *inst = tsp_instance_create(nodes, 14);
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    HardFixingConfig cfg = {
        .time_limit = 10.0,
        .fixing_rate = 0.7,
        .heuristic_time_ratio = 0.3,
        .heuristic_type = TABU,
        .seed = 42,
        .heuristic_args = NULL
    };

    TspAlgorithm hf = hard_fixing_create(cfg);
    tsp_algorithm_run(&hf, inst, sol, rec);

    FeasibilityResult res = tsp_solution_check_feasibility(sol);
    assert(res == FEASIBLE);

    double cost = tsp_solution_get_cost(sol);
    printf("    HardFixing Solution Cost: %.4f (Opt: 30.8785)\n", cost);

    assert(cost < 35.0);

    tsp_algorithm_destroy(&hf);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);

    printf("  [HardFixing] Passed.\n");
}

void run_hard_fixing_tests(void) {
    printf("[Hard Fixing] Running tests...\n");
    test_hard_fixing_burma14();
    printf("[Hard Fixing] All tests passed.\n");
}