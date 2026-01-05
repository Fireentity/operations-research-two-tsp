#include "test_instances.h"
#include "benders_loop.h"
#include "branch_and_cut.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

static void test_benders_burma14(void) {
#ifdef ENABLE_CPLEX
    printf("  [Exact] Testing Benders on Burma14...\n");
    TspInstance *inst = create_burma14_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    BendersConfig cfg = {.max_iterations = 100, .time_limit = TIME_LIMIT_EXACT};
    TspAlgorithm algo = benders_create(cfg);

    tsp_algorithm_run(&algo, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: %.4f)\n", cost, BURMA14_OPT_COST);

    assert(fabs(cost - BURMA14_OPT_COST) < EPSILON_EXACT);

    tsp_algorithm_destroy(&algo);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

static void test_benders_geometric(void) {
#ifdef ENABLE_CPLEX
    printf("  [Exact] Testing Benders on Geometric (Square)...\n");
    TspInstance *inst = create_square_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    BendersConfig cfg = {.max_iterations = 50, .time_limit = TIME_LIMIT_EXACT};
    TspAlgorithm algo = benders_create(cfg);

    tsp_algorithm_run(&algo, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    assert(fabs(tsp_solution_get_cost(sol) - 40.0) < EPSILON_EXACT);

    tsp_algorithm_destroy(&algo);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

static void test_benders_fallback_timeout(void) {
#ifdef ENABLE_CPLEX
    printf("  [Exact] Testing Benders Fallback (Random 100 + Timeout)...\n");
    TspInstance *inst = create_random_instance_100();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    // Force timeout to trigger robust subtour extraction and naive merge
    BendersConfig cfg = {.max_iterations = 500, .time_limit = 0.001};
    TspAlgorithm algo = benders_create(cfg);

    tsp_algorithm_run(&algo, inst, sol, rec);

    FeasibilityResult res = tsp_solution_check_feasibility(sol);
    if (res != FEASIBLE) {
        printf("    [ERROR] Fallback solution invalid: %s\n", feasibility_result_to_string(res));
    }
    assert(res == FEASIBLE);
    assert(tsp_solution_get_cost(sol) > 0.0);

    tsp_algorithm_destroy(&algo);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

static void test_bc_burma14(void) {
#ifdef ENABLE_CPLEX
    printf("  [Exact] Testing Branch & Cut on Burma14...\n");
    TspInstance *inst = create_burma14_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(100);

    BranchCutConfig cfg = {.time_limit = TIME_LIMIT_EXACT, .num_threads = 1};
    TspAlgorithm algo = branch_and_cut_create(cfg);

    tsp_algorithm_run(&algo, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    double cost = tsp_solution_get_cost(sol);
    printf("    Cost: %.4f (Opt: %.4f)\n", cost, BURMA14_OPT_COST);

    assert(fabs(cost - BURMA14_OPT_COST) < EPSILON_EXACT);

    tsp_algorithm_destroy(&algo);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

static void test_bc_geometric(void) {
#ifdef ENABLE_CPLEX
    printf("  [Exact] Testing Branch & Cut on Geometric (Hexagon)...\n");
    TspInstance *inst = create_hexagon_instance();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    BranchCutConfig cfg = {.time_limit = TIME_LIMIT_EXACT, .num_threads = 1};
    TspAlgorithm algo = branch_and_cut_create(cfg);

    tsp_algorithm_run(&algo, inst, sol, rec);

    assert(tsp_solution_check_feasibility(sol) == FEASIBLE);
    assert(fabs(tsp_solution_get_cost(sol) - 6.0) < EPSILON_EXACT);

    tsp_algorithm_destroy(&algo);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

static void test_bc_fallback_timeout(void) {
#ifdef ENABLE_CPLEX
    printf("  [Exact] Testing B&C Fallback (Random 100 + Timeout)...\n");
    TspInstance *inst = create_random_instance_100();
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    // Force timeout to trigger LP-guided GRASP fallback
    BranchCutConfig cfg = {.time_limit = 0.001, .num_threads = 1};
    TspAlgorithm algo = branch_and_cut_create(cfg);

    tsp_algorithm_run(&algo, inst, sol, rec);

    FeasibilityResult res = tsp_solution_check_feasibility(sol);
    if (res != FEASIBLE) {
        printf("    [ERROR] Fallback solution invalid: %s\n", feasibility_result_to_string(res));
    }
    assert(res == FEASIBLE);
    assert(tsp_solution_get_cost(sol) > 0.0);

    tsp_algorithm_destroy(&algo);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
#endif
}

void run_exact_tests(void) {
    printf("[Exact Algorithms] Running tests...\n");
#ifdef ENABLE_CPLEX
    test_benders_burma14();
    test_benders_geometric();
    test_benders_fallback_timeout();
    test_bc_burma14();
    test_bc_geometric();
    test_bc_fallback_timeout();
#else
    printf("  [SKIP] CPLEX not enabled.\n");
#endif
    printf("[Exact Algorithms] All tests passed.\n");
}