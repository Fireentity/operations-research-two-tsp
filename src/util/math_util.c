#include "math_util.h"
#include "tsp_solution.h"

double calculate_tour_cost(const unsigned long *const tour, const unsigned long number_of_nodes, const double *const edge_cost_array) {
    double cost = 0;
    for (int i = 0; i < tour_array_size(number_of_nodes); i++) {
        cost += edge_cost_array[tour[i] * number_of_nodes + tour[i + 1]];
    }
    return cost;
}


