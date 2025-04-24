#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <time_limiter.h>
#include <tsp_math_util.h>
#include "algorithms.h"
#include "c_util.h"

#define EPSILON 1e-10

static double optimize_with_two_opt(int* tour,
                                  const int number_of_nodes,
                                  const double* edge_cost_array,
                                  const TimeLimiter* time_limiter,
                                  const double epsilon)
{
    bool improved = true; // Flag to track if any improvement was made
    double delta = 0;

    // Continue iterating until no further improvements are possible
    while (improved)
    {
        improved = false;
        delta += two_opt(tour, number_of_nodes, edge_cost_array, time_limiter, epsilon);
        improved = delta < 0;

        if (time_limiter->is_time_over(time_limiter))
        {
            return delta;
        }
    }

    return delta;
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
    optimize_with_two_opt(tour, number_of_nodes, edge_cost_array, time_limiter, EPSILON);

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
    delta += two_opt(tour, number_of_nodes, edge_cost_array, time_limiter, EPSILON);

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
    delta = optimize_with_two_opt(tour, number_of_nodes, edge_cost_array, time_limiter, EPSILON);

    assert(int_arrays_equal(tour, result_tour,tour_size));
    assert(delta == delta_result);
}

int main()
{
    test_case1();
    test_case2();
    testcase3();
    return 0;
}
