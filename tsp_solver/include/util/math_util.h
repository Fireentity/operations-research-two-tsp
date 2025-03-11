#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include "../../tsp_algo_lib/include/tsp_instance.h"

double calculate_tour_cost(const unsigned long *tour, unsigned long number_of_nodes, const double *edge_cost_array);
double random01();

#endif //MATH_UTIL_H
