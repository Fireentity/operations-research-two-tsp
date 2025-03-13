#include <math.h>
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

double* init_edge_cost_array(const Node* nodes, const int number_of_nodes)
{
    double* const edge_cost_array = calloc(number_of_nodes * number_of_nodes, sizeof(double));
    const double edge_array_size = number_of_nodes * number_of_nodes;
    for (int i = 0; i < edge_array_size; i++)
    {
        const int row = i / number_of_nodes;
        const int colum = i % number_of_nodes;
        const double dx = nodes[row].x - nodes[colum].x;
        const double dy = nodes[row].y - nodes[colum].y;
        edge_cost_array[i] = sqrt(dx * dx + dy * dy);
    }

    return edge_cost_array;
}
