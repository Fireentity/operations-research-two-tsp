#include "local_branching_test.h"
#include "local_branching.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define EPSILON_TEST 1e-4

void test_local_branching_burma14(void) {
    printf("  [LocalBranching] Testing burma14 (k=5, VNS warm-start)...\n");

    Node nodes[14] = {
        {16.47, 96.10}, {16.47, 94.44}, {20.09, 92.54}, {22.39, 93.37},
        {25.23, 97.24}, {22.00, 96.05}, {20.47, 97.02}, {17.20, 96.29},
        {16.30, 97.38}, {14.05, 98.12}, {16.53, 97.38}, {21.52, 95.59},
        {19.41, 97.13}, {20.09, 94.55}
    };

    TspInstance *inst = tsp_instance_create(nodes, 14);
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    LocalBranchingConfig cfg = {
        .time_limit = 5.0,
        .k = 5,
        .heuristic_time_ratio = 0.2,
        .heuristic_type = TABU,
        .seed = 42,
        .heuristic_args = NULL
    };

    TspAlgorithm lb = local_branching_create(cfg);
    tsp_algorithm_run(&lb, inst, sol, rec);

    FeasibilityResult res = tsp_solution_check_feasibility(sol);
    assert(res == FEASIBLE);

    double cost = tsp_solution_get_cost(sol);
    printf("    LocalBranching Solution Cost: %.4f (Opt: 30.8785)\n", cost);

    assert(cost < 34.0);

    tsp_algorithm_destroy(&lb);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);

    printf("  [LocalBranching] Passed.\n");
}

void run_local_branching_tests(void) {
    printf("[Local Branching] Running tests...\n");
    test_local_branching_burma14();
    printf("[Local Branching] All tests passed.\n");
}
