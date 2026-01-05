#include "test_instances.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "variable_neighborhood_search.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"

static void test_vns_burma14(void) {
    printf("  [VNS] Testing Burma14...\n");
    TspInstance *inst = create_burma14_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    VNSConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .min_k = 2,
        .max_k = 5,
        .kick_repetition = 2,
        .max_stagnation = 50,
        .seed = 42
    };

    TspAlgorithm vns = vns_create(config);
    tsp_algorithm_run(&vns, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: %.4f)\n", cost, BURMA14_OPT_COST);

    assert(fabs(cost - BURMA14_OPT_COST) < EPSILON_HEURISTIC);

    tsp_algorithm_destroy(&vns);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_vns_hexagon(void) {
    printf("  [VNS] Testing Hexagon...\n");
    TspInstance *inst = create_hexagon_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    VNSConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .min_k = 2,
        .max_k = 3,
        .kick_repetition = 1,
        .max_stagnation = 20,
        .seed = 42
    };

    TspAlgorithm vns = vns_create(config);
    tsp_algorithm_run(&vns, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: 6.0)\n", cost);

    assert(fabs(cost - 6.0) < EPSILON_EXACT);

    tsp_algorithm_destroy(&vns);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_vns_random_100(void) {
    printf("  [VNS] Testing Random 100...\n");
    TspInstance *inst = create_random_instance_100();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(100);

    VNSConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .min_k = 3,
        .max_k = 10,
        .kick_repetition = 5,
        .max_stagnation = 100,
        .seed = 12345
    };

    TspAlgorithm vns = vns_create(config);
    tsp_algorithm_run(&vns, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    assert(tsp_solution_get_cost(sol) > 0.0);

    tsp_algorithm_destroy(&vns);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

void run_vns_tests(void) {
    printf("[VNS] Running tests...\n");
    test_vns_burma14();
    test_vns_hexagon();
    test_vns_random_100();
    printf("[VNS] All tests passed.\n");
}