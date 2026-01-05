#include "test_instances.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "grasp.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"

static void test_grasp_burma14(void) {
    printf("  [GRASP] Testing Burma14...\n");
    TspInstance *inst = create_burma14_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(100);

    GraspConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .rcl_size = 3,
        .probability = 0.5,
        .max_stagnation = 50,
        .seed = 42
    };

    TspAlgorithm grasp = grasp_create(config);
    tsp_algorithm_run(&grasp, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);

    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: %.4f)\n", cost, BURMA14_OPT_COST);

    assert(fabs(cost - BURMA14_OPT_COST) < EPSILON_HEURISTIC);

    tsp_algorithm_destroy(&grasp);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_grasp_square(void) {
    printf("  [GRASP] Testing Square...\n");
    TspInstance *inst = create_square_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    GraspConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .rcl_size = 2,
        .probability = 0.5,
        .max_stagnation = 10,
        .seed = 42
    };

    TspAlgorithm grasp = grasp_create(config);
    tsp_algorithm_run(&grasp, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    assert(fabs(tsp_solution_get_cost(sol) - 40.0) < EPSILON_EXACT);

    tsp_algorithm_destroy(&grasp);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_grasp_random_100(void) {
    printf("  [GRASP] Testing Random 100...\n");
    TspInstance *inst = create_random_instance_100();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(100);

    GraspConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .rcl_size = 5,
        .probability = 0.2,
        .max_stagnation = 100,
        .seed = 12345
    };

    TspAlgorithm grasp = grasp_create(config);
    tsp_algorithm_run(&grasp, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    assert(tsp_solution_get_cost(sol) > 0.0);

    tsp_algorithm_destroy(&grasp);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_grasp_reproducibility(void) {
    printf("  [GRASP] Testing Reproducibility...\n");
    TspInstance *inst = create_random_instance_100();

    GraspConfig config = {
        .time_limit = 1.0,
        .rcl_size = 5,
        .probability = 0.5,
        .max_stagnation = 10,
        .seed = 999
    };

    // Run 1
    TspSolution *sol1 = tsp_solution_create(inst);
    TspAlgorithm grasp1 = grasp_create(config);
    tsp_algorithm_run(&grasp1, inst, sol1, NULL);
    double cost1 = tsp_solution_get_cost(sol1);

    // Run 2
    TspSolution *sol2 = tsp_solution_create(inst);
    TspAlgorithm grasp2 = grasp_create(config);
    tsp_algorithm_run(&grasp2, inst, sol2, NULL);
    double cost2 = tsp_solution_get_cost(sol2);

    assert(fabs(cost1 - cost2) < EPSILON_EXACT);

    tsp_algorithm_destroy(&grasp1);
    tsp_algorithm_destroy(&grasp2);
    tsp_solution_destroy(sol1);
    tsp_solution_destroy(sol2);
    tsp_instance_destroy(inst);
}

void run_grasp_tests(void) {
    printf("[GRASP] Running tests...\n");
    test_grasp_burma14();
    test_grasp_square();
    test_grasp_random_100();
    test_grasp_reproducibility();
    printf("[GRASP] All tests passed.\n");
}