#include "grasp_nn_helpers_test.h"
#include <assert.h>
#include <stdio.h>
#include "constructive.h"

static void create_dummy_costs(double *costs, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            costs[i * n + j] = (i == j) ? 0.0 : 10.0;
        }
    }
}

static void test_grasp_construction_basic(void) {
    const int n = 5;
    double costs[25];
    create_dummy_costs(costs, n);

    int tour[6]; // n + 1
    double cost = 0.0;

    // Run with defined RCL parameters
    // RCL Size = 3, Probability = 0.5
    int res = grasp_nearest_neighbor_tour(0, tour, n, costs, &cost, 3, 0.5);

    assert(res == 0);
    assert(tour[0] == 0);
    assert(tour[n] == 0); // Closed
    assert(cost > 0.0);
}

static void test_grasp_invalid_start_node(void) {
    const int n = 5;
    double costs[25];
    create_dummy_costs(costs, n);
    int tour[6];
    double cost;

    // Start node 10 is out of bounds [0, 5)
    // RCL params don't matter here as it should fail early
    int res = grasp_nearest_neighbor_tour(10, tour, n, costs, &cost, 3, 0.5);
    assert(res == -1);
}

void run_grasp_nn_helpers_tests(void) {
    printf("[GRASP Helper] Running tests...\n");
    test_grasp_construction_basic();
    test_grasp_invalid_start_node();
    printf("[GRASP Helper] Passed.\n");
}