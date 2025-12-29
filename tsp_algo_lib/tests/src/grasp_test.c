#include "grasp_test.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "grasp.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "constants.h"

#define NODES 20
#define TIME_LIMIT 1.0

static void test_grasp_reproducibility(void) {
    printf("\t[GRASP] Testing Reproducibility (Seeding)...\n");

    TspGenerationArea area = {.x_square = 0, .y_square = 0, .square_side = 100};
    TspInstance *inst = tsp_instance_create_random(50, area);

    GraspConfig config = {
        .rcl_size = 5,
        .probability = 0.5,
        .max_stagnation = 10,
        .time_limit = 1.0,
        .seed = 12345
    };

    TspSolution *sol1 = tsp_solution_create(inst);
    CostRecorder *rec1 = cost_recorder_create(10);
    TspAlgorithm grasp1 = grasp_create(config);
    tsp_algorithm_run(&grasp1, inst, sol1, rec1);
    double cost1 = tsp_solution_get_cost(sol1);

    TspSolution *sol2 = tsp_solution_create(inst);
    CostRecorder *rec2 = cost_recorder_create(10);
    TspAlgorithm grasp2 = grasp_create(config);
    tsp_algorithm_run(&grasp2, inst, sol2, rec2);
    double cost2 = tsp_solution_get_cost(sol2);

    assert(fabs(cost1 - cost2) < EPSILON);

    tsp_algorithm_destroy(&grasp1);
    tsp_algorithm_destroy(&grasp2);
    tsp_solution_destroy(sol1);
    tsp_solution_destroy(sol2);
    cost_recorder_destroy(rec1);
    cost_recorder_destroy(rec2);
    tsp_instance_destroy(inst);
}

void run_grasp_tests(void) {
    printf("[GRASP] Running tests...\n");

    TspGenerationArea area = {.x_square = 0, .y_square = 0, .square_side = 100};
    TspInstance *instance = tsp_instance_create_random(NODES, area);
    TspSolution *solution = tsp_solution_create(instance);
    CostRecorder *recorder = cost_recorder_create(100);

    GraspConfig config = {
        .time_limit = TIME_LIMIT,
        .rcl_size = 3,
        .probability = 0.8,
        .max_stagnation = 50
    };
    TspAlgorithm grasp = grasp_create(config);

    tsp_algorithm_run(&grasp, instance, solution, recorder);

    assert(tsp_solution_get_cost(solution) > 0);
    assert(tsp_solution_check_feasibility(solution) == FEASIBLE);

    int tour[NODES + 1];
    tsp_solution_get_tour(solution, tour);
    assert(tour[0] == tour[NODES]);

    tsp_algorithm_destroy(&grasp);
    cost_recorder_destroy(recorder);
    tsp_solution_destroy(solution);
    tsp_instance_destroy(instance);

    test_grasp_reproducibility();

    printf("[GRASP] Passed.\n");
}
#undef NODES
#undef TIME_LIMIT
