#include "nn_test.h"
#include <assert.h>
#include <stdio.h>
#include "nearest_neighbor.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "cost_recorder.h"

#define NODES 20
#define TIME_LIMIT 1.0

void run_nn_tests(void) {
    printf("[NN] Running tests...\n");

    // 1. Setup Instance
    TspGenerationArea area = {.x_square = 0, .y_square = 0, .square_side = 100};
    TspInstance *instance = tsp_instance_create_random(NODES, 42, area);
    TspSolution *solution = tsp_solution_create(instance);
    CostRecorder *recorder = cost_recorder_create(100);

    // 2. Setup Algorithm
    NNConfig config = {.time_limit = TIME_LIMIT};
    TspAlgorithm nn = nn_create(config);

    // 3. Execution
    tsp_algorithm_run(&nn, instance, solution, recorder);

    // 4. Validation
    double cost = tsp_solution_get_cost(solution);
    assert(cost > 0.0 && "Cost must be positive");

    int tour[NODES + 1];
    tsp_solution_get_tour(solution, tour);
    assert(tour[0] == tour[NODES] && "Tour must be closed");

    // Check tour validity (no duplicates)
    assert(tsp_solution_check_feasibility(solution) == FEASIBLE);

    // 5. Cleanup
    tsp_algorithm_destroy(&nn);
    cost_recorder_destroy(recorder);
    tsp_solution_destroy(solution);
    tsp_instance_destroy(instance);

    printf("[NN] Passed.\n");
}
#undef NODES
#undef TIME_LIMIT