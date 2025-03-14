#include <stdio.h>
#include <c_util.h>
#include <tsp_instance.h>
#include <tsp_math_util.h>

inline double n_opt(const int number_of_segments,
                    int* tour,
                    const int* segments,
                    const double* edge_cost_array,
                    const int number_of_nodes);

inline double n_opt(const int number_of_segments,
                    int* tour,
                    const int* segments,
                    const double* edge_cost_array,
                    const int number_of_nodes)
{
    double delta = 0;
    for (int i = 1; i < number_of_segments; i++)
    {
        delta -= edge_cost_array[tour[segments[i]] + tour[segments[i + 1]] * number_of_nodes];
        REVERSE_ARRAY(tour, segments[i], segments[i+1]);
        delta += edge_cost_array[tour[segments[i]] + tour[segments[i + 1]] * number_of_nodes];
    }
    return delta;
}


inline double compute_cost(const int* tour,
                           const int number_of_nodes,
                           const double* edge_cost_array)
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
    int tour[] = {0, 1, 2, 3, 0};

    const int number_of_nodes = sizeof(nodes) / sizeof(nodes[0]);
    const double* edge_cost_array = init_edge_cost_array(nodes, number_of_nodes);
    const int segments[] = {0,1,2,3};
    n_opt(2,tour,segments,edge_cost_array,number_of_nodes);
    for (int i = 0; i < number_of_nodes; i++)
    {
        printf("%d ", tour[i]);
    }
}

int main()
{
    test_case1();
    printf("All tests passed.\n");
    return 0;
}
