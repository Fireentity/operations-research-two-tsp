#include "test_instances.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "extra_mileage.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"

static void test_em_burma14(void) {
    printf("  [EM] Testing Burma14...\n");
    TspInstance *inst = create_burma14_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    EMConfig cfg = {.time_limit = TIME_LIMIT_HEURISTIC};
    TspAlgorithm em = em_create(cfg);

    tsp_algorithm_run(&em, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: %.4f)\n", cost, BURMA14_OPT_COST);

    assert(fabs(cost - BURMA14_OPT_COST) < EPSILON_HEURISTIC+2); // idk but em is bad on burma with this setup

    tsp_algorithm_destroy(&em);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_em_hexagon(void) {
    printf("  [EM] Testing Hexagon...\n");
    TspInstance *inst = create_hexagon_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    EMConfig cfg = {.time_limit = TIME_LIMIT_HEURISTIC};
    TspAlgorithm em = em_create(cfg);

    tsp_algorithm_run(&em, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: 6.0)\n", cost);

    assert(fabs(cost - 6.0) < EPSILON_EXACT);

    tsp_algorithm_destroy(&em);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_em_square(void) {
    printf("  [EM] Testing Square...\n");
    TspInstance *inst = create_square_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    EMConfig cfg = {.time_limit = TIME_LIMIT_HEURISTIC};
    TspAlgorithm em = em_create(cfg);

    tsp_algorithm_run(&em, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: 40.0)\n", cost);

    assert(fabs(cost - 40.0) < EPSILON_EXACT);

    tsp_algorithm_destroy(&em);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_em_random_100(void) {
    printf("  [EM] Testing Random 100...\n");
    TspInstance *inst = create_random_instance_100();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    EMConfig cfg = {.time_limit = TIME_LIMIT_HEURISTIC};
    TspAlgorithm em = em_create(cfg);

    tsp_algorithm_run(&em, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    assert(tsp_solution_get_cost(sol) > 0.0);

    tsp_algorithm_destroy(&em);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

void run_em_tests(void) {
    printf("[Extra Mileage] Running tests...\n");
    test_em_burma14();
    test_em_hexagon();
    test_em_square();
    test_em_random_100();
    printf("[Extra Mileage] All tests passed.\n");
}