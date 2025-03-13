#ifndef MATH_UTIL_H
#define MATH_UTIL_H
#include <tsp_instance.h>

double calculate_tour_cost(const int *tour, int number_of_nodes, const double *edge_cost_array);
double normalized_rand();
double* init_edge_cost_array(const Node* nodes, int number_of_nodes);
#endif //MATH_UTIL_H
