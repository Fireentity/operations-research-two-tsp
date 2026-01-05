#include "test_instances.h"
#include "hard_fixing.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

static void test_hard_fixing_burma14(void) {
#ifdef ENABLE_CPLEX
    printf("  [HardFixing] Testing Burma14...\n");
    TspInstance *inst = create_burma14_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    HardFixingConfig cfg = {
        .time_limit = TIME_LIMIT_EXACT,
        .fixing_rate = 0.7,
        .heuristic_time_ratio = 0.2,
        .time_slice_factor = 0.1,
        .min_time_slice = 1.0,
        .heuristic_type = TABU,
        .seed = 42
    };

    TspAlgorithm hf = hard_fixing_create(cfg);
    tsp_algorithm_run(&hf, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: %.4f)\n", cost, BURMA14_OPT_COST);

    assert(fabs(cost - BURMA14_OPT_COST) < EPSILON_EXACT);

    tsp_algorithm_destroy(&hf);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

static void test_hard_fixing_hexagon(void) {
#ifdef ENABLE_CPLEX
    printf("  [HardFixing] Testing Hexagon...\n");
    TspInstance *inst = create_hexagon_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    HardFixingConfig cfg = {
        .time_limit = TIME_LIMIT_EXACT,
        .fixing_rate = 0.5,
        .heuristic_time_ratio = 0.1,
        .time_slice_factor = 0.5,
        .min_time_slice = 0.5,
        .heuristic_type = VNS,
        .seed = 42
    };

    TspAlgorithm hf = hard_fixing_create(cfg);
    tsp_algorithm_run(&hf, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: 6.0)\n", cost);

    assert(fabs(cost - 6.0) < EPSILON_EXACT);

    tsp_algorithm_destroy(&hf);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

static void test_hard_fixing_random_100(void) {
#ifdef ENABLE_CPLEX
    printf("  [HardFixing] Testing Random 100...\n");
    TspInstance *inst = create_random_instance_100();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(100);

    HardFixingConfig cfg = {
        .time_limit = TIME_LIMIT_EXACT,
        .fixing_rate = 0.9,
        .heuristic_time_ratio = 0.2,
        .time_slice_factor = 0.2,
        .min_time_slice = 2.0,
        .heuristic_type = VNS,
        .seed = 12345
    };

    TspAlgorithm hf = hard_fixing_create(cfg);
    tsp_algorithm_run(&hf, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    assert(tsp_solution_get_cost(sol) > 0.0);

    tsp_algorithm_destroy(&hf);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

void run_hard_fixing_tests(void) {
    printf("[Hard Fixing] Running tests...\n");
#ifdef ENABLE_CPLEX
    test_hard_fixing_burma14();
    test_hard_fixing_hexagon();
    test_hard_fixing_random_100();
#else
    printf("  [SKIP] CPLEX not enabled.\n");
#endif
    printf("[Hard Fixing] All tests passed.\n");
}