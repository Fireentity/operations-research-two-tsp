#include <stdlib.h>
#include "math_util.h"

double calculate_tour_cost(const unsigned long *const tour, const unsigned long number_of_nodes, const double *const edge_cost_array) {
    double cost = 0;
    for (unsigned long i = 0; i < number_of_nodes; i++) {
        cost += edge_cost_array[tour[i] * number_of_nodes + tour[i + 1]];
    }
    return cost;
}

double random01() { return (double) rand() / RAND_MAX; }
