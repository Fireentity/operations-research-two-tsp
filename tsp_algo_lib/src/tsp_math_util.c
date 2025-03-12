#include <stdlib.h>
#include <tsp_math_util.h>

double calculate_tour_cost(const int* const tour,
                           const int number_of_nodes,
                           const double* const edge_cost_array)
{
    double cost = 0;
    for (int i = 0; i < number_of_nodes; i++)
    {
        cost += edge_cost_array[tour[i] * number_of_nodes + tour[i + 1]];
    }
    return cost;
}

double normalized_rand() { return (double)rand() / RAND_MAX; }
