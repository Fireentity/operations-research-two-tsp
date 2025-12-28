#include "hard_fixing_test.h"
#include "hard_fixing.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "feasibility_result.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void test_hard_fixing_basic(void) {
    printf("  [HardFixing] Testing basic execution (20 nodes, NN heuristic)...\n");
    
    // Create random instance
    TspGenerationArea area = {0, 0, 100};
    TspInstance *inst = tsp_instance_create_random(20, area);
    TspSolution *sol = tsp_solution_create(inst);
    CostRecorder *rec = cost_recorder_create(10);

    // Config: Use NN as base, fix 80%, short time limit
    // We expect the wrapper to fallback gracefully if CPLEX is missing
    HardFixingConfig cfg = {
        .time_limit = 1.0,
        .fixing_rate = 0.8,
        .heuristic_time_ratio = 0.5,
        .heuristic_type = HF_HEURISTIC_NN,
        .seed = 42,
        .heuristic_args = NULL
    };

    TspAlgorithm hf = hard_fixing_create(cfg);
    tsp_algorithm_run(&hf, inst, sol, rec);

    // Validations
    FeasibilityResult res = tsp_solution_check_feasibility(sol);
    if (res != FEASIBLE) {
        printf("    [Error] Solution invalid: %s\n", feasibility_result_to_string(res));
    }
    assert(res == FEASIBLE);

    double cost = tsp_solution_get_cost(sol);
    assert(cost > 0.0);
    
    printf("    HardFixing Solution Cost: %.2f\n", cost);

    tsp_algorithm_destroy(&hf);
    cost_recorder_destroy(rec);
    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
    
    printf("  [HardFixing] Passed.\n");
}

void run_hard_fixing_tests(void) {
    printf("[Hard Fixing] Running tests...\n");
    test_hard_fixing_basic();
    printf("[Hard Fixing] All tests passed.\n");
}