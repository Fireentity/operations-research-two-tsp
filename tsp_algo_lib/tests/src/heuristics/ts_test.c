#include "test_instances.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "tabu_search.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"

static void test_ts_burma14(void) {
    printf("  [Tabu] Testing Burma14...\n");
    TspInstance *inst = create_burma14_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    TabuConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .min_tenure = 2,
        .max_tenure = 5,
        .max_stagnation = 50,
        .seed = 42
    };

    TspAlgorithm tabu = tabu_create(config);
    tsp_algorithm_run(&tabu, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: %.4f)\n", cost, BURMA14_OPT_COST);

    assert(fabs(cost - BURMA14_OPT_COST) < EPSILON_HEURISTIC);

    tsp_algorithm_destroy(&tabu);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_ts_square(void) {
    printf("  [Tabu] Testing Square...\n");
    TspInstance *inst = create_square_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    TabuConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .min_tenure = 2,
        .max_tenure = 5,
        .max_stagnation = 20,
        .seed = 42
    };

    TspAlgorithm tabu = tabu_create(config);
    tsp_algorithm_run(&tabu, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: 40.0)\n", cost);

    assert(fabs(cost - 40.0) < EPSILON_EXACT);

    tsp_algorithm_destroy(&tabu);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_ts_random_100(void) {
    printf("  [Tabu] Testing Random 100...\n");
    TspInstance *inst = create_random_instance_100();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(100);

    TabuConfig config = {
        .time_limit = TIME_LIMIT_HEURISTIC,
        .min_tenure = 5,
        .max_tenure = 20,
        .max_stagnation = 100,
        .seed = 12345
    };

    TspAlgorithm tabu = tabu_create(config);
    tsp_algorithm_run(&tabu, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    assert(tsp_solution_get_cost(sol) > 0.0);

    tsp_algorithm_destroy(&tabu);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

void run_ts_tests(void) {
    printf("[Tabu Search] Running tests...\n");
    test_ts_burma14();
    test_ts_square();
    test_ts_random_100();
    printf("[Tabu Search] All tests passed.\n");
}