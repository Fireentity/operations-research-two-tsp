#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tsp_math_util.h>

#include "c_util.h"

void test_case3()
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
    const int segments[] = {2, 5};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_segments = sizeof(segments) / sizeof(segments[0]);

    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    const double delta = compute_n_opt_cost(number_of_segments, tour, segments, edge_cost_array, number_of_nodes);
    compute_n_opt_move(number_of_segments, tour, segments, number_of_nodes);
    const double delta_result = 2 * sqrt(2) - 2;

    assert(arrays_equal(tour, result_tour,tour_size, sizeof(tour[0])));
    assert(delta == delta_result);

    free(edge_cost_array);
}

void test_case4()
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
    const int segments[] = {2, 5};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_segments = sizeof(segments) / sizeof(segments[0]);

    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    const double delta = compute_n_opt_cost(number_of_segments, tour, segments, edge_cost_array, number_of_nodes);
    compute_n_opt_move(number_of_segments, tour, segments, number_of_nodes);
    const double delta_result = 2 * sqrt(2) - 2;

    assert(arrays_equal(tour, result_tour,tour_size, sizeof(tour[0])));
    assert(delta == delta_result);

    free(edge_cost_array);
}

void test_case5()
{
    const Node nodes[] = {
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1}
    };
    int tour[] = {0, 1, 2, 3, 0};
    const int result_tour[] = {1, 0, 2, 3, 1};
    const int edges_to_remove[] = {3, 1};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_segments = sizeof(edges_to_remove) / sizeof(edges_to_remove[0]);

    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    const double delta = compute_n_opt_cost(number_of_segments, tour, edges_to_remove, edge_cost_array, number_of_nodes);
    compute_n_opt_move(number_of_segments, tour, edges_to_remove, number_of_nodes);
    const double delta_result = 2 * sqrt(2) - 2;

    assert(arrays_equal(tour, result_tour,tour_size, sizeof(tour[0])));
    assert(delta == delta_result);

    free(edge_cost_array);
}

void test_case1()
{
    const Node nodes[] = {
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1}
    };
    int tour[] = {0, 1, 2, 3, 0};
    const int result_tour[] = {0, 1, 3, 2, 0};
    const int edges_to_remove[] = {1, 3};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_segments = sizeof(edges_to_remove) / sizeof(edges_to_remove[0]);

    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    const double delta = compute_n_opt_cost(number_of_segments, tour, edges_to_remove, edge_cost_array, number_of_nodes);
    compute_n_opt_move(number_of_segments, tour, edges_to_remove, number_of_nodes);
    const double delta_result = 2 * sqrt(2) - 2;

    assert(arrays_equal(tour, result_tour,tour_size, sizeof(tour[0])));
    assert(delta == delta_result);

    free(edge_cost_array);
}

void test_case2()
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
    const int edges_to_remove[] = {1, 3, 5, 7};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_segments = sizeof(edges_to_remove) / sizeof(edges_to_remove[0]);

    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    const double delta = compute_n_opt_cost(number_of_segments, tour, edges_to_remove, edge_cost_array, number_of_nodes);
    compute_n_opt_move(number_of_segments, tour, edges_to_remove,number_of_nodes);
    const double delta_result = 2 * sqrt(5);

    assert(arrays_equal(tour, result_tour,tour_size, sizeof(tour[0])));
    assert(delta == delta_result);

    free(edge_cost_array);
}

int main()
{
    test_case1();
    test_case2();
    test_case3();
    test_case4();
    test_case5();
    return 0;
}
