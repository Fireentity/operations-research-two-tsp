#include <assert.h>
#include <float.h>
#include <math.h>
#include <time_limiter.h>
#include <tsp_math_util.h>

#include "c_util.h"

static double two_opt(int* tour,
                      const int number_of_nodes,
                      const double* edge_cost_array,
                      double* cost,
                      const TimeLimiter* time_limiter)
{
    // Iterate over possible starting indices for a two-opt move
    for (int i = 0; i < number_of_nodes - 1; i++)
    {
        if (time_limiter->is_time_over(time_limiter))
        {
            return 1;
        }

        // Iterate over possible end indices for the segment to be reversed
        for (int k = i + 2; k < number_of_nodes; k++)
        {
            // Define the endpoints of the segment to remove
            const int edge_to_remove[] = {i, k};
            // Compute the cost difference for the proposed two-opt move
            const double delta = compute_n_opt_cost(2, tour, edge_to_remove, edge_cost_array, number_of_nodes);
            // If the move does not improve the tour cost, skip it
            if (delta >= 0)
                continue;

            // Update the tour cost with the improvement
            *cost += delta;
            // Apply the two-opt move to modify the tour
            compute_n_opt_move(2, tour, edge_to_remove, number_of_nodes);
            return delta;
        }
    }

    return 1;
}

static void optimize_with_two_opt(int* tour,
                                  const int number_of_nodes,
                                  const double* edge_cost_array,
                                  double* cost,
                                  const TimeLimiter* time_limiter)
{
    bool improved = true; // Flag to track if any improvement was made

    // Continue iterating until no further improvements are possible
    while (improved)
    {
        improved = false;
        improved = two_opt(tour, number_of_nodes, edge_cost_array, cost, time_limiter) < 0;

        if (time_limiter->is_time_over(time_limiter))
        {
            return;
        }
    }
}

void testcase3()
{
    const Node nodes[] = {
        {0, 0},
        {1, 1},
        {1, 0.5},
        {1, 0},
        {0.5, 1},
        {0, 1},
    };
    int tour[] = {0, 1, 2, 3, 4, 5, 0};
    const int result_tour[] = {0, 3, 2, 1, 4, 5, 0};
    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    double delta = 0.0;
    const double delta_result = 1.5 - sqrt(5) / 2 - sqrt(2);
    const TimeLimiter* time_limiter = init_time_limiter(DBL_MAX);
    optimize_with_two_opt(tour, number_of_nodes, edge_cost_array, &delta, time_limiter);

    assert(int_arrays_equal(tour, result_tour,tour_size));
    assert(fabs(delta_result - delta) < 1e-10);

    free(edge_cost_array);
}

void test_case1()
{
    const Node nodes[] = {
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
    };
    int tour[] = {0, 1, 2, 3, 0};
    const int result_tour[] = {0, 1, 2, 3, 0};
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    double delta = 0.0;
    const double delta_result = 0;
    const TimeLimiter* time_limiter = init_time_limiter(DBL_MAX);
    two_opt(tour, number_of_nodes, edge_cost_array, &delta, time_limiter);

    assert(int_arrays_equal(tour, result_tour,tour_size));
    assert(delta_result == delta);
}

void test_case2()
{
    const Node nodes[] = {
        {0, 0},
        {1, 0},
        {0, 1},
        {1, 1}
    };
    int tour[] = {0, 1, 2, 3, 0};
    const int result_tour[] = {0, 1, 3, 2, 0};
    const int tour_size = sizeof(tour) / sizeof(tour[0]);
    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    double delta = 0.0;
    const double delta_result = 2 - 2 * sqrt(2);
    const TimeLimiter* time_limiter = init_time_limiter(DBL_MAX);
    optimize_with_two_opt(tour, number_of_nodes, edge_cost_array, &delta, time_limiter);

    assert(int_arrays_equal(tour, result_tour,tour_size));
    assert(delta == delta_result);
}

int main()
{
    // test_case1();
    // test_case2();
    testcase3();
    return 0;
}
