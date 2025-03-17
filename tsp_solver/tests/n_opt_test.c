#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tsp_math_util.h>
#include "c_util.h"

// Compare two Node arrays element-wise based on x and y values
DEFINE_ARRAYS_EQUAL(Node, node, a.x == b.x && a.y == b.y);

/**
 * Test reversing a segment in a square tour.
 * Initial tour: 0->1->2->3->0; remove edges at indices 1 and 3.
 * Expected result: 0->1->3->2->0 with cost change 2*sqrt(2)-2.
 */
void test_square_reverse_segment()
{
    const Node nodes[] = {
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1}
    };
    // Initial tour and expected tour after 2-opt move.
    int tour[] = {0, 1, 2, 3, 0};
    const int result_tour[] = {0, 1, 3, 2, 0};
    // Edges to remove for the 2-opt move.
    const int edges_to_remove[] = {1, 3};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_segments = sizeof(edges_to_remove) / sizeof(edges_to_remove[0]);

    // Precompute distances between nodes.
    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    // Compute cost difference for the 2-opt move.
    const double delta = compute_n_opt_cost(number_of_segments, tour, edges_to_remove, edge_cost_array, number_of_nodes);
    // Execute the 2-opt move.
    compute_n_opt_move(number_of_segments, tour, edges_to_remove, number_of_nodes);
    const double delta_result = 2 * sqrt(2) - 2;

    // Verify tour and cost difference.
    assert(node_arrays_equal(tour, result_tour, tour_size));
    assert(delta == delta_result);

    free(edge_cost_array);
}

/**
 * Test reversing multiple segments in an octagon tour.
 * Initial tour: 0->1->2->3->4->5->6->7->0; remove edges at indices 1,3,5,7.
 * Expected result: 0->1->3->2->5->4->7->6->0 with cost change 2*sqrt(5).
 */
void test_octagon_multiple_segment_reverse()
{
    const Node nodes[] = {
        {1, 0},
        {2, 0},
        {3, 1},
        {3, 2},
        {2, 3},
        {1, 3},
        {0, 2},
        {0, 1}
    };
    int tour[] = {0, 1, 2, 3, 4, 5, 6, 7, 0};
    const int result_tour[] = {0, 1, 3, 2, 5, 4, 7, 6, 0};
    // Edges to remove for multiple 2-opt moves.
    const int edges_to_remove[] = {1, 3, 5, 7};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_segments = sizeof(edges_to_remove) / sizeof(edges_to_remove[0]);

    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    const double delta = compute_n_opt_cost(number_of_segments, tour, edges_to_remove, edge_cost_array, number_of_nodes);
    compute_n_opt_move(number_of_segments, tour, edges_to_remove, number_of_nodes);
    const double delta_result = 2 * sqrt(5);

    assert(node_arrays_equal(tour, result_tour, tour_size));
    assert(delta == delta_result);

    free(edge_cost_array);
}

/**
 * Test reversing a segment in a hexagon tour (case 1).
 * Initial tour: 0->1->2->3->4->5->0; remove edges at indices 2 and 5.
 * Expected result: 0->1->2->5->4->3->0 with cost change 2*sqrt(2)-2.
 */
void test_hexagon_reverse_segment_case1()
{
    const Node nodes[] = {
        {0, 0},
        {0.5, 0},
        {1, 0},
        {1, 1},
        {0.5, 1},
        {0, 1}
    };
    int tour[] = {0, 1, 2, 3, 4, 5, 0};
    const int result_tour[] = {0, 1, 2, 5, 4, 3, 0};
    // Segments (edges) to remove.
    const int segments[] = {2, 5};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_segments = sizeof(segments) / sizeof(segments[0]);

    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    const double delta = compute_n_opt_cost(number_of_segments, tour, segments, edge_cost_array, number_of_nodes);
    compute_n_opt_move(number_of_segments, tour, segments, number_of_nodes);
    const double delta_result = 2 * sqrt(2) - 2;

    assert(node_arrays_equal(tour, result_tour, tour_size));
    assert(delta == delta_result);

    free(edge_cost_array);
}

/**
 * Test reversing a segment in a hexagon tour (case 2) with an alternate initial tour.
 * Initial tour: 0->4->2->3->1->5->0; remove edges at indices 2 and 5.
 * Expected result: 0->4->2->5->1->3->0 with cost change 2*sqrt(2)-2.
 */
void test_hexagon_reverse_segment_case2()
{
    const Node nodes[] = {
        {0, 0},
        {0.5, 0},
        {1, 0},
        {1, 1},
        {0.5, 1},
        {0, 1}
    };
    int tour[] = {0, 4, 2, 3, 1, 5, 0};
    const int result_tour[] = {0, 4, 2, 5, 1, 3, 0};
    // Segments (edges) to remove.
    const int segments[] = {2, 5};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_segments = sizeof(segments) / sizeof(segments[0]);

    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    const double delta = compute_n_opt_cost(number_of_segments, tour, segments, edge_cost_array, number_of_nodes);
    compute_n_opt_move(number_of_segments, tour, segments, number_of_nodes);
    const double delta_result = 2 * sqrt(2) - 2;

    assert(node_arrays_equal(tour, result_tour, tour_size));
    assert(delta == delta_result);

    free(edge_cost_array);
}

/**
 * Test reversing a segment in a square tour with alternate removal order.
 * Initial tour: 0->1->2->3->0; remove edges at indices 3 and 1.
 * Expected result: 1->0->2->3->1 with cost change 2*sqrt(2)-2.
 */
void test_square_alternate_segment_reverse()
{
    const Node nodes[] = {
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1}
    };
    int tour[] = {0, 1, 2, 3, 0};
    const int result_tour[] = {1, 0, 2, 3, 1};
    // Edges to remove in alternate order.
    const int edges_to_remove[] = {3, 1};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_segments = sizeof(edges_to_remove) / sizeof(edges_to_remove[0]);

    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    const double delta = compute_n_opt_cost(number_of_segments, tour, edges_to_remove, edge_cost_array, number_of_nodes);
    compute_n_opt_move(number_of_segments, tour, edges_to_remove, number_of_nodes);
    const double delta_result = 2 * sqrt(2) - 2;

    assert(node_arrays_equal(tour, result_tour, tour_size));
    assert(delta == delta_result);

    free(edge_cost_array);
}

int main()
{
    test_square_reverse_segment();
    test_octagon_multiple_segment_reverse();
    test_hexagon_reverse_segment_case1();
    test_hexagon_reverse_segment_case2();
    test_square_alternate_segment_reverse();
    return 0;
}
