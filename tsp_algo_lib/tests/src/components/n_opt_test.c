#include "test_instances.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "c_util.h"
#include "tsp_math.h"
#include "tsp_tour.h"

#define ASSERT_DBL_EQ(a, b) assert(fabs((a) - (b)) < EPSILON_EXACT)

static void test_square_reverse_segment(void) {
    TspInstance *inst = create_square_instance();
    const double *costs = tsp_instance_get_cost_matrix(inst);
    int n = 4;

    // Initial: 0->1->2->3->0 (Perimeter)
    int tour[] = {0, 1, 2, 3, 0};

    // Move: 2-opt removing edges (0,1) and (2,3)
    // Effectively swaps to diagonals
    const int edges_to_remove[] = {0, 2};
    const int n_segments = 2;

    // Expected delta: 2 diagonals (20*sqrt(2)) - 2 sides (20)
    double expected_delta = 20.0 * sqrt(2.0) - 20.0;

    double delta = compute_n_opt_cost(n_segments, tour, edges_to_remove, costs, n);
    ASSERT_DBL_EQ(delta, expected_delta);

    compute_n_opt_move(n_segments, tour, edges_to_remove, n);

    const int expected_tour[] = {0, 2, 1, 3, 0};
    assert(int_arrays_equal(tour, expected_tour, 5));

    tsp_instance_destroy(inst);
}

static void test_hexagon_reverse_segment(void) {
    TspInstance *inst = create_hexagon_instance();
    // We don't use costs here, just checking the topological swap
    int n = 6;

    int tour[] = {0, 1, 2, 3, 4, 5, 0};

    // Remove edges at index 2 (2->3) and 5 (5->0)
    // Segment reversed: tour[3]..tour[5] -> 3,4,5 becomes 5,4,3
    const int edges_to_remove[] = {2, 5};

    compute_n_opt_move(2, tour, edges_to_remove, n);

    const int expected_tour[] = {0, 1, 2, 5, 4, 3, 0};
    assert(int_arrays_equal(tour, expected_tour, 7));

    tsp_instance_destroy(inst);
}

void run_n_opt_tests(void) {
    printf("[N-Opt] Running tests...\n");
    test_square_reverse_segment();
    test_hexagon_reverse_segment();
    printf("[N-Opt] Passed.\n");
}