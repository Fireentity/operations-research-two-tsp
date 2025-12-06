#include "em_test.h"
#include <assert.h>
#include <stdio.h>
#include "extra_mileage.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "cost_recorder.h"

#define NODES 20
#define TIME_LIMIT 1.0

void run_em_tests(void) {
    printf("[Extra Mileage] Running tests...\n");

    // 1. Setup
    TspGenerationArea area = {.x_square = 0, .y_square = 0, .square_side = 100};
    TspInstance *instance = tsp_instance_create_random(NODES, area);
    TspSolution *solution = tsp_solution_create(instance);
    CostRecorder *recorder = cost_recorder_create(100);

    // 2. Config
    EMConfig config = { .time_limit = TIME_LIMIT };
    TspAlgorithm em = em_create(config);

    // 3. Run
    tsp_algorithm_run(&em, instance, solution, recorder);

    // 4. Assertions
    assert(tsp_solution_get_cost(solution) > 0);
    assert(tsp_solution_check_feasibility(solution) == FEASIBLE);

    int tour[NODES + 1];
    tsp_solution_get_tour(solution, tour);
    assert(tour[0] == tour[NODES]);

    // 5. Cleanup
    tsp_algorithm_destroy(&em);
    cost_recorder_destroy(recorder);
    tsp_solution_destroy(solution);
    tsp_instance_destroy(instance);

    printf("[Extra Mileage] Passed.\n");
}
#undef NODES
#undef TIME_LIMIT