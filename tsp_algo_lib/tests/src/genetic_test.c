#include "genetic_test.h"
#include <assert.h>
#include <stdio.h>
#include "genetic.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "cost_recorder.h"

#define NODES 50
#define TIME_LIMIT 1.0

void run_genetic_tests(void) {
    printf("[Genetic] Running tests...\n");

    // 1. Setup
    // Using a consistent seed for reproducibility
    TspGenerationArea area = {.x_square = 0, .y_square = 0, .square_side = 100};
    TspInstance *instance = tsp_instance_create_random(NODES, area);
    TspSolution *solution = tsp_solution_create(instance);
    CostRecorder *recorder = cost_recorder_create(100);

    // 2. Config
    GeneticConfig config = {
        .time_limit = TIME_LIMIT,
        .population_size = 100,       // Moderate population for testing
        .elite_count = 2,             // Keep top 2
        .mutation_rate = 0.1,         // 10% mutation chance
        .crossover_cut_min_ratio = 25,
        .crossover_cut_max_ratio = 75,
        .init_grasp_rcl_size = 5,
        .init_grasp_prob = 0.2,
        .init_grasp_percent = 90,
        .tournament_size = 5,
        .seed = 12345
    };
    
    TspAlgorithm ga = genetic_create(config);

    // 3. Run
    // This will trigger initialization (GRASP/Random), Selection, Crossover, Repair, and 2-Opt
    tsp_algorithm_run(&ga, instance, solution, recorder);

    // 4. Assertions
    // Ensure the solution is structurally valid (closed loop, no duplicates)
    FeasibilityResult feasibility = tsp_solution_check_feasibility(solution);
    if (feasibility != FEASIBLE) {
        printf("[Genetic] Feasibility check failed: %s\n", feasibility_result_to_string(feasibility));
    }
    assert(feasibility == FEASIBLE);
    
    // Ensure we actually calculated a cost
    double cost = tsp_solution_get_cost(solution);
    assert(cost > 0.0);

    // Check tour structure manually (Start == End)
    int tour[NODES + 1];
    tsp_solution_get_tour(solution, tour);
    assert(tour[0] == tour[NODES]);

    printf("[Genetic] Final cost found: %.2f\n", cost);

    // 5. Cleanup
    tsp_algorithm_destroy(&ga);
    cost_recorder_destroy(recorder);
    tsp_solution_destroy(solution);
    tsp_instance_destroy(instance);

    printf("[Genetic] Passed.\n");
}
#undef NODES
#undef TIME_LIMIT