#include "nn_test.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "nearest_neighbor.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "constants.h"

#define NODES 20
#define TIME_LIMIT 1.0

static void test_nn_tiny_instance(void) {
    printf("\t[NN] Testing Tiny Instance (N=2)...\n");

    Node nodes[] = {{0, 0}, {10, 0}};
    TspInstance *inst = tsp_instance_create(nodes, 2);
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    NNConfig config = {.time_limit = 1.0};
    TspAlgorithm nn = nn_create(config);

    tsp_algorithm_run(&nn, inst, sol, rec);

    double cost = tsp_solution_get_cost(sol);
    assert(fabs(cost - 20.0) < EPSILON);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);

    tsp_algorithm_destroy(&nn);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

void run_nn_tests(void) {
    printf("[NN] Running tests...\n");

    TspGenerationArea area = {.x_square = 0, .y_square = 0, .square_side = 100};
    TspInstance *instance = tsp_instance_create_random(NODES, area);
    TspSolution *solution = tsp_solution_create(instance);
    CostRecorder *recorder = cost_recorder_create(100);

    NNConfig config = {.time_limit = TIME_LIMIT};
    TspAlgorithm nn = nn_create(config);

    tsp_algorithm_run(&nn, instance, solution, recorder);

    double cost = tsp_solution_get_cost(solution);
    assert(cost > 0.0);

    int tour[NODES + 1];
    tsp_solution_get_tour(solution, tour);
    assert(tour[0] == tour[NODES]);

    assert(tsp_solution_check_feasibility(solution) == FEASIBLE);

    tsp_algorithm_destroy(&nn);
    cost_recorder_destroy(recorder);
    tsp_solution_destroy(solution);
    tsp_instance_destroy(instance);

    test_nn_tiny_instance();

    printf("[NN] Passed.\n");
}
#undef NODES
#undef TIME_LIMIT
