#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <c_util.h>
#include <tsp_instance.h>
#include <tsp_math_util.h>
#include <tsp_solution.h>

static void rotate_left(int* arr, int len, int shift)
{
    if (shift <= 0 || shift >= len) return;
    REVERSE_ARRAY(arr, 0, len - 1);
    REVERSE_ARRAY(arr, 0, len - shift - 1);
    REVERSE_ARRAY(arr, len - shift, len - 1);
}


#define N_OPT(tour, number_of_segments, ...) \
tour[FIRST(__VA_ARGS_)] =

void n_opt(int* tour,
           const int* segments,
           const int number_of_segments)
{
    for (int i = 0; i < number_of_segments; i++)
    {
        SWAP(tour[segments[i+1]], tour[segments[i+number_of_segments]]);
    }

    for (int i = 0; i < number_of_segments; i++)
    {
        REVERSE_ARRAY(tour, segments[i], segments[i+1]);
    }
}

#define _2_OPT(a, b, c, d) \
SWAP(b, c);                  \
SWAP(c, d)

#define _3_OPT(a, b, c, d, e, f) \
SWAP(b, d);                         \
SWAP(c, e);                         \
SWAP(d, f)

#define _4_OPT(a, b, c, d, e, f, g, h) \
SWAP(b, e);                                \
SWAP(c, f);                                \
SWAP(d, g);                                \
SWAP(e, h)

#define N_OPT(N, ...) _##N_OPT(__VA_ARGS__)


void n_opt(int* tour,
           const int s_1,
           const int s_2,
           const int s_3,
           const int s_4,
           const int s_5,
           const int s_6,
           const int number_of_segments)
{
    //SWAP(s_<i+1>,s_<i+ number_of_segments/2>)
    SWAP(tour[s_3], tour[s_6]);

    for (int i = 0; i < number_of_segments; i++)
    {
        REVERSE_ARRAY(tour, segments[i], segments[i+1]);
    }
}

double compute_cost(const int* tour, int number_of_nodes, const double* edge_cost_array)
{
    double total = 0.0;
    for (int i = 0; i < number_of_nodes - 1; i++)
        total += edge_cost_array[tour[i] * number_of_nodes + tour[i + 1]];
    total += edge_cost_array[tour[number_of_nodes - 1] * number_of_nodes + tour[0]];
    return total;
}

void test_case1()
{
    const Node nodes[] = {
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
    };
    int tour[] = {0, 2, 1, 3, 0};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    const int* edges =



    double cost = 0.0;

    calculate_tour_cost(tour, number_of_nodes, edge_cost_array);

    n_opt_move(tour, 4, edge_cost_array, &cost, 2, );

    for (int i = 0; i < number_of_nodes; i++)
        assert(tour[i] == expected[i]);

    assert(cost == compute_cost(expected, number_of_nodes, cost_matrix));
    free(cost_matrix);
}

void test_case2()
{
    // n = 2: reverse segment from index 2 to 3
    int tour[4] = {0, 1, 2, 3};
    int expected[4] = {0, 1, 3, 2};
    int number_of_nodes = 4;
    double* cost_matrix = create_cost_matrix(number_of_nodes);
    double cost;
    int n = 2;
    int edges[2] = {1, 3};

    n_opt_move(tour, number_of_nodes, cost_matrix, &cost, n, edges);
    for (int i = 0; i < number_of_nodes; i++)
        assert(tour[i] == expected[i]);
    assert(cost == compute_cost(expected, number_of_nodes, cost_matrix));
    free(cost_matrix);
}

void test_case3()
{
    // n = 3: wrap-around reversal
    int tour[6] = {0, 1, 2, 3, 4, 5};
    int expected[6] = {0, 2, 3, 4, 5, 1};
    int number_of_nodes = 6;
    double* cost_matrix = create_cost_matrix(number_of_nodes);
    double cost;
    int n = 3;
    int edges[3] = {4, 1, 3};

    n_opt_move(tour, number_of_nodes, cost_matrix, &cost, n, edges);
    for (int i = 0; i < number_of_nodes; i++)
        assert(tour[i] == expected[i]);
    assert(cost == compute_cost(expected, number_of_nodes, cost_matrix));
    free(cost_matrix);
}

void test_case4()
{
    // Single node
    int tour[1] = {0};
    int expected[1] = {0};
    int number_of_nodes = 1;
    double* cost_matrix = create_cost_matrix(number_of_nodes);
    double cost;
    int n = 2;
    int edges[2] = {0, 0};

    n_opt_move(tour, number_of_nodes, cost_matrix, &cost, n, edges);
    assert(tour[0] == expected[0]);
    assert(cost == compute_cost(expected, number_of_nodes, cost_matrix));
    free(cost_matrix);
}

int main()
{
    test_case1();
    test_case2();
    test_case3();
    test_case4();
    printf("All tests passed.\n");
    return 0;
}
