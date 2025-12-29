#include "ts_test.h"
#include <assert.h>
#include <stdio.h>
#include "tabu_search.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "cost_recorder.h"

#define NODES 20
#define TIME_LIMIT 1.0

void run_ts_tests(void) {
    printf("[Tabu Search] Running tests...\n");

    // 1. Setup
    TspGenerationArea area = {.x_square = 0, .y_square = 0, .square_side = 100};
    TspInstance *instance = tsp_instance_create_random(NODES, area);
    TspSolution *solution = tsp_solution_create(instance);
    CostRecorder *recorder = cost_recorder_create(100);

    // 2. Config
    TabuConfig config = {
        .time_limit = TIME_LIMIT,
        .min_tenure = 5,
        .max_tenure = 10,
        .max_stagnation = 20
    };
    TspAlgorithm tabu = tabu_create(config);

    // 3. Run
    tsp_algorithm_run(&tabu, instance, solution, recorder);

    // 4. Assertions
    assert(tsp_solution_check_feasibility(solution) == FEASIBLE);
    assert(tsp_solution_get_cost(solution) > 0);

    // 5. Cleanup
    tsp_algorithm_destroy(&tabu);
    cost_recorder_destroy(recorder);
    tsp_solution_destroy(solution);
    tsp_instance_destroy(instance);

    printf("[Tabu Search] Passed.\n");
}
#undef NODES
#undef TIME_LIMIT
