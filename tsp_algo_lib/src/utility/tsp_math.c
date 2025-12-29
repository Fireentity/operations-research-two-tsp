#include <math.h>
#include <stdlib.h>
#include "c_util.h"
#include "tsp_instance.h"
#include "logger.h"

double calculate_tour_cost(const int *const tour,
                           const int number_of_nodes,
                           const double *const edge_cost_array) {
    double cost = 0;
    for (int i = 0; i < number_of_nodes; i++) {
        cost += edge_cost_array[tour[i] * number_of_nodes + tour[i + 1]];
    }
    return cost;
}

double *init_edge_cost_array(const Node *nodes, const int number_of_nodes) {
    if_verbose(VERBOSE_DEBUG, "\tAllocating edge cost array for %d nodes (%lu bytes).\n",
               number_of_nodes, (unsigned long) (number_of_nodes * number_of_nodes * sizeof(double)));

    double *const edge_cost_array = tsp_calloc(number_of_nodes * number_of_nodes, sizeof(double));

    const double edge_array_size = number_of_nodes * number_of_nodes;

    if_verbose(VERBOSE_DEBUG, "\tCalculating Euclidean distances for %d nodes...\n", number_of_nodes);
    for (int i = 0; i < edge_array_size; i++) {
        const int row = i / number_of_nodes;
        const int colum = i % number_of_nodes;
        if (row == colum) continue; // Cost is 0, already set by tsp_calloc

        const double dx = nodes[row].x - nodes[colum].x;
        const double dy = nodes[row].y - nodes[colum].y;
        edge_cost_array[i] = sqrt(dx * dx + dy * dy);
    }
    return edge_cost_array;
}
