#include "test_instances.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "genetic.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"

static void test_genetic_burma14(void) {
    printf("  [Genetic] Testing Burma14...\n");
    TspInstance *inst = create_burma14_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    GeneticConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .population_size = 200,
        .elite_count = 2,
        .mutation_rate = 0.1,
        .crossover_cut_min_ratio = 25,
        .crossover_cut_max_ratio = 75,
        .init_grasp_rcl_size = 5,
        .init_grasp_prob = 0.2,
        .init_grasp_percent = 90,
        .tournament_size = 5,
        .seed = 42
    };

    TspAlgorithm ga = genetic_create(config);
    tsp_algorithm_run(&ga, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);

    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: %.4f)\n", cost, BURMA14_OPT_COST);

    assert(fabs(cost - BURMA14_OPT_COST) < EPSILON_HEURISTIC);

    tsp_algorithm_destroy(&ga);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_genetic_circle(void) {
    printf("  [Genetic] Testing Circle (Geometric)...\n");
    TspInstance *inst = create_circle_instance(20, 100.0);
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    GeneticConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .population_size = 100,
        .elite_count = 2,
        .mutation_rate = 0.1,
        .crossover_cut_min_ratio = 25,
        .crossover_cut_max_ratio = 75,
        .init_grasp_rcl_size = 5,
        .init_grasp_prob = 0.2,
        .init_grasp_percent = 90,
        .tournament_size = 3,
        .seed = 12345
    };

    TspAlgorithm ga = genetic_create(config);
    tsp_algorithm_run(&ga, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);

    // Perimeter approx 2*PI*R = 628.3
    printf("    Cost: %.4f (Exp: < 630.0)\n", cost);
    assert(cost < 635.0);

    tsp_algorithm_destroy(&ga);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_genetic_random_100(void) {
    printf("  [Genetic] Testing Random 100...\n");
    TspInstance *inst = create_random_instance_100();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    GeneticConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .population_size = 100,
        .elite_count = 2,
        .mutation_rate = 0.1,
        .crossover_cut_min_ratio = 25,
        .crossover_cut_max_ratio = 75,
        .init_grasp_rcl_size = 5,
        .init_grasp_prob = 0.2,
        .init_grasp_percent = 90,
        .tournament_size = 3,
        .seed = 12345
    };

    TspAlgorithm ga = genetic_create(config);
    tsp_algorithm_run(&ga, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    assert(tsp_solution_get_cost(sol) > 0.0);

    tsp_algorithm_destroy(&ga);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

void run_genetic_tests(void) {
    printf("[Genetic] Running tests...\n");
    test_genetic_burma14();
    test_genetic_circle();
    test_genetic_random_100();
    printf("[Genetic] All tests passed.\n");
}