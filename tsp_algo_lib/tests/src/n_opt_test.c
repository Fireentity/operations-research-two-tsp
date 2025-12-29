#include "n_opt_test.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "c_util.h"
#include "tsp_math.h"
#include "tsp_tour.h" // Added: needed for compute_n_opt_move

// Helper to assert double equality with epsilon
#define ASSERT_DBL_EQ(a, b) assert(fabs((a) - (b)) < 1e-9)

void test_square_reverse_segment(void) {
    // Square 1x1
    const Node nodes[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    const int n = 4;
    double *costs = init_edge_cost_array(nodes, n);

    // Initial: 0->1->2->3->0 (Perimeter, cost 4)
    int tour[] = {0, 1, 2, 3, 0};

    // Move: 2-opt removing edges (0,1) and (2,3)
    // Indices in tour array: 0 and 2.
    // Edge 0 connects tour[0]-tour[1] (0-1)
    // Edge 2 connects tour[2]-tour[3] (2-3)
    const int edges_to_remove[] = {0, 2};
    const int n_segments = 2;

    // Expected delta: New edges are (0,2) [diag] and (1,3) [diag].
    // Old cost: 1 + 1 = 2. New cost: sqrt(2) + sqrt(2).
    // Delta = 2*sqrt(2) - 2.
    double expected_delta = 2.0 * sqrt(2.0) - 2.0;

    double delta = compute_n_opt_cost(n_segments, tour, edges_to_remove, costs, n);
    ASSERT_DBL_EQ(delta, expected_delta);

    // Perform move
    compute_n_opt_move(n_segments, tour, edges_to_remove, n);

    // Expected Tour: 0 -> 2 -> 1 -> 3 -> 0
    const int expected_tour[] = {0, 2, 1, 3, 0};
    assert(int_arrays_equal(tour, expected_tour, 5));

    tsp_free(costs);
}

void test_hexagon_reverse_segment(void) {
    // Hexagon shape
    const Node nodes[] = {
        {0, 0}, {0.5, 0}, {1, 0},
        {1, 1}, {0.5, 1}, {0, 1}
    };
    const int n = 6;
    double *costs = init_edge_cost_array(nodes, n);

    int tour[] = {0, 1, 2, 3, 4, 5, 0};
    // Remove edges at index 2 (2->3) and 5 (5->0)
    // Reconnects 2->5 and 3->0?
    // Wait, 2-opt reverses segment BETWEEN cuts.
    // Cut 1: after tour[2]. Cut 2: after tour[5].
    // Segment reversed: tour[3]...tour[5].
    const int edges_to_remove[] = {2, 5};

    // Run Logic
    compute_n_opt_move(2, tour, edges_to_remove, n);

    // Expected: 0->1->2 -> 5->4->3 -> 0
    const int expected_tour[] = {0, 1, 2, 5, 4, 3, 0};
    assert(int_arrays_equal(tour, expected_tour, 7));

    tsp_free(costs);
}

void run_n_opt_tests(void) {
    printf("[N-Opt] Running tests...\n");
    test_square_reverse_segment();
    test_hexagon_reverse_segment();
    printf("[N-Opt] Passed.\n");
}
#undef ASSERT_DBL_EQ
