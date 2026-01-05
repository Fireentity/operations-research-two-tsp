#include "test_instances.h"
#include "local_branching.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

static void test_lb_burma14(void) {
#ifdef ENABLE_CPLEX
    printf("  [LocalBranching] Testing Burma14...\n");
    TspInstance *inst = create_burma14_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    LocalBranchingConfig cfg = {
        .time_limit = TIME_LIMIT_EXACT,
        .k = 5,
        .heuristic_time_ratio = 0.2,
        .heuristic_type = TABU,
        .seed = 42
    };

    TspAlgorithm lb = local_branching_create(cfg);
    tsp_algorithm_run(&lb, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);

    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: %.4f)\n", cost, BURMA14_OPT_COST);

    assert(fabs(cost - BURMA14_OPT_COST) < EPSILON_EXACT);

    tsp_algorithm_destroy(&lb);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

static void test_lb_hexagon(void) {
#ifdef ENABLE_CPLEX
    printf("  [LocalBranching] Testing Hexagon...\n");
    TspInstance *inst = create_hexagon_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    LocalBranchingConfig cfg = {
        .time_limit = TIME_LIMIT_EXACT,
        .k = 3,
        .heuristic_time_ratio = 0.1,
        .heuristic_type = VNS,
        .seed = 42
    };

    TspAlgorithm lb = local_branching_create(cfg);
    tsp_algorithm_run(&lb, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: 6.0)\n", cost);

    assert(fabs(cost - 6.0) < EPSILON_EXACT);

    tsp_algorithm_destroy(&lb);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

static void test_lb_random_100(void) {
#ifdef ENABLE_CPLEX
    printf("  [LocalBranching] Testing Random 100...\n");
    TspInstance *inst = create_random_instance_100();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(100);

    LocalBranchingConfig cfg = {
        .time_limit = TIME_LIMIT_EXACT,
        .k = 10,
        .heuristic_time_ratio = 0.2,
        .heuristic_type = VNS,
        .seed = 12345
    };

    TspAlgorithm lb = local_branching_create(cfg);
    tsp_algorithm_run(&lb, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    assert(tsp_solution_get_cost(sol) > 0.0);

    tsp_algorithm_destroy(&lb);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

void run_local_branching_tests(void) {
    printf("[Local Branching] Running tests...\n");
#ifdef ENABLE_CPLEX
    test_lb_burma14();
    test_lb_hexagon();
    test_lb_random_100();
#else
    printf("  [SKIP] CPLEX not enabled.\n");
#endif
    printf("[Local Branching] All tests passed.\n");
}