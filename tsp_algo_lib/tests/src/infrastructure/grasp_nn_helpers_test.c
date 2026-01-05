#include "test_instances.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "constructive.h"
#include "random.h"
#include "tsp_math.h"

static RandomState rng;

static void test_grasp_helper_basic_square(void) {
    printf("  [GRASP Helper] Testing on Square Instance...\n");

    // Use the shared square instance to test the constructive function directly
    TspInstance *inst = create_square_instance();
    const double *costs = tsp_instance_get_cost_matrix(inst);
    int n = 4;

    int tour[5]; // n+1
    double cost = 0.0;

    // RCL=1 makes it deterministic NN
    int res = grasp_nearest_neighbor_tour(0, tour, n, costs, &cost, 1, 0.0, &rng);

    assert(res == 0);
    assert(tour[0] == 0);
    assert(tour[4] == 0);

    // On a square, NN is usually optimal (perimeter)
    assert(fabs(cost - 40.0) < EPSILON_EXACT);

    tsp_instance_destroy(inst);
}

static void test_grasp_helper_invalid_input(void) {
    printf("  [GRASP Helper] Testing Invalid Inputs...\n");

    TspInstance *inst = create_square_instance();
    const double *costs = tsp_instance_get_cost_matrix(inst);
    int n = 4;
    int tour[5];
    double cost;

    // Invalid start node
    assert(grasp_nearest_neighbor_tour(-1, tour, n, costs, &cost, 3, 0.5, &rng) == -1);
    assert(grasp_nearest_neighbor_tour(4, tour, n, costs, &cost, 3, 0.5, &rng) == -1); // Index 4 is out of 0-3

    // Invalid RCL size
    assert(grasp_nearest_neighbor_tour(0, tour, n, costs, &cost, 0, 0.5, &rng) == -1);

    tsp_instance_destroy(inst);
}

void run_grasp_nn_helpers_tests(void) {
    printf("[GRASP Helper] Running tests...\n");
    random_init(&rng, 12345);
    test_grasp_helper_basic_square();
    test_grasp_helper_invalid_input();
    printf("[GRASP Helper] All tests passed.\n");
}