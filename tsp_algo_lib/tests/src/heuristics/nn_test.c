#include "test_instances.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "nearest_neighbor.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"

static void test_nn_burma14(void) {
    printf("  [NN] Testing Burma14...\n");
    TspInstance *inst = create_burma14_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    NNConfig config = {.time_limit = TIME_LIMIT_HEURISTIC, .num_threads = 1};
    TspAlgorithm nn = nn_create(config);

    tsp_algorithm_run(&nn, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: %.4f)\n", cost, BURMA14_OPT_COST);

    // NN is greedy, likely won't be optimal, but must be valid
    assert(cost > BURMA14_OPT_COST);

    tsp_algorithm_destroy(&nn);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_nn_square(void) {
    printf("  [NN] Testing Square...\n");
    TspInstance *inst = create_square_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    NNConfig config = {.time_limit = TIME_LIMIT_HEURISTIC, .num_threads = 1};
    TspAlgorithm nn = nn_create(config);

    tsp_algorithm_run(&nn, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);

    // On a square, NN usually finds the perimeter (40.0)
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: 40.0)\n", cost);
    assert(fabs(cost - 40.0) < EPSILON_EXACT);

    tsp_algorithm_destroy(&nn);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_nn_random_100(void) {
    printf("  [NN] Testing Random 100...\n");
    TspInstance *inst = create_random_instance_100();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(100);

    NNConfig config = {.time_limit = TIME_LIMIT_HEURISTIC, .num_threads = 1};
    TspAlgorithm nn = nn_create(config);

    tsp_algorithm_run(&nn, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    assert(tsp_solution_get_cost(sol) > 0.0);

    tsp_algorithm_destroy(&nn);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

void run_nn_tests(void) {
    printf("[NN] Running tests...\n");
    test_nn_burma14();
    test_nn_square();
    test_nn_random_100();
    printf("[NN] All tests passed.\n");
}