#include "test_instances.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "tsp_math.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include "c_util.h"

static void test_euclidean_distance(void) {
    printf("\t[Utility] Testing Euclidean calculation...\n");
    Node nodes[] = {{0.0, 0.0}, {3.0, 0.0}, {0.0, 4.0}};
    int n = 3;
    double *matrix = init_edge_cost_array(nodes, n);

    assert(fabs(matrix[0*n + 1] - 3.0) < EPSILON_EXACT);
    assert(fabs(matrix[0*n + 2] - 4.0) < EPSILON_EXACT);
    assert(fabs(matrix[1*n + 2] - 5.0) < EPSILON_EXACT);

    tsp_free(matrix);
}

static void test_tour_cost_calculation(void) {
    printf("\t[Utility] Testing Tour Cost Sum...\n");
    TspInstance *inst = create_square_instance();
    const double *costs = tsp_instance_get_cost_matrix(inst);
    int n = 4;

    int tour[] = {0, 1, 2, 3, 0};
    double cost = calculate_tour_cost(tour, n, costs);

    assert(fabs(cost - 40.0) < EPSILON_EXACT);

    tsp_instance_destroy(inst);
}

static void test_solution_update_logic(void) {
    printf("\t[Utility] Testing Solution Update Logic...\n");
    TspInstance *inst = create_square_instance();
    TspSolution *sol = tsp_solution_create(inst);
    double initial_cost = tsp_solution_get_cost(sol);

    int new_tour[] = {0, 1, 3, 2, 0};
    double bad_cost = initial_cost + 100.0;

    bool updated = tsp_solution_update_if_better(sol, new_tour, bad_cost);
    assert(updated == false);
    assert(fabs(tsp_solution_get_cost(sol) - initial_cost) < EPSILON_EXACT);

    double good_cost = initial_cost - 1.0;
    updated = tsp_solution_update_if_better(sol, new_tour, good_cost);
    assert(updated == true);
    assert(fabs(tsp_solution_get_cost(sol) - good_cost) < EPSILON_EXACT);

    tsp_solution_destroy(sol);
    tsp_instance_destroy(inst);
}

static void test_recorder_resize(void) {
    printf("\t[Utility] Testing CostRecorder resize...\n");
    CostRecorder *rec = cost_recorder_create(2);
    for (int i = 0; i < 1000; i++) {
        cost_recorder_add(rec, (double) i);
    }
    assert(cost_recorder_get_count(rec) == 1000);
    const double *data = cost_recorder_get_costs(rec);
    assert(fabs(data[999] - 999.0) < EPSILON_EXACT);
    cost_recorder_destroy(rec);
}

void run_utility_tests(void) {
    printf("[Utility] Running tests...\n");
    test_euclidean_distance();
    test_tour_cost_calculation();
    test_solution_update_logic();
    test_recorder_resize();
    printf("[Utility] Passed.\n");
}