#include "exact_test.h"
#include "benders_loop.h"
#include "branch_and_cut.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

#define EPSILON_TEST 1e-4

void test_benders_small_random() {
#ifdef ENABLE_CPLEX
    printf("  [Exact] Testing Benders on small random instance (10 nodes)...\n");
    TspGenerationArea area = {0, 0, 100};
    TspInstance *inst = tsp_instance_create_random(10, area);
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    BendersConfig cfg = { .max_iterations = 100, .time_limit = 10.0 };
    TspAlgorithm algo = benders_create(cfg);

    tsp_algorithm_run(&algo, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    assert(cost > 0.0);
    printf("    Benders found solution with cost: %.2f\n", cost);

    tsp_algorithm_destroy(&algo);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#else
    printf("  [Exact] SKIP Benders test (CPLEX not enabled).\n");
#endif
}

void test_branch_and_cut_burma14_embedded() {
#ifdef ENABLE_CPLEX
    printf("  [Exact] Testing Branch & Cut on embedded burma14...\n");

    Node nodes[14] = {
        {16.47, 96.10},
        {16.47, 94.44},
        {20.09, 92.54},
        {22.39, 93.37},
        {25.23, 97.24},
        {22.00, 96.05},
        {20.47, 97.02},
        {17.20, 96.29},
        {16.30, 97.38},
        {14.05, 98.12},
        {16.53, 97.38},
        {21.52, 95.59},
        {19.41, 97.13},
        {20.09, 94.55}
    };

    TspInstance *inst = tsp_instance_create(nodes, 14);
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(100);

    BranchCutConfig cfg = {
        .time_limit = 30.0,
        .num_threads = 1
    };

    TspAlgorithm algo = branch_and_cut_create(cfg);
    tsp_algorithm_run(&algo, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);

    double cost = tsp_solution_get_cost(sol);
    printf("    B&C Solution Cost: %.6f\n", cost);

    // Optimum burma14 with EUC_2D is something like 30.8785
    assert(fabs(cost - 30.8785) < EPSILON_TEST);

    tsp_algorithm_destroy(&algo);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#else
    printf("  [Exact] SKIP Branch&Cut test (CPLEX not enabled).\n");
#endif
}

void run_exact_tests(void) {
    printf("[Exact Algorithms] Running tests...\n");
    test_benders_small_random();
    test_branch_and_cut_burma14_embedded();
    printf("[Exact Algorithms] All tests passed.\n");
}