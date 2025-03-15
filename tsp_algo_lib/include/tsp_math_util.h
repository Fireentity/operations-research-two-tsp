#ifndef MATH_UTIL_H
#define MATH_UTIL_H
#include <tsp_instance.h>

double calculate_tour_cost(const int* tour, int number_of_nodes, const double* edge_cost_array);
double normalized_rand();
double* init_edge_cost_array(const Node* nodes, int number_of_nodes);
double compute_n_opt_cost(int number_of_segments,
                          int tour[],
                          const int* edges_to_remove,
                          const double edge_cost_array[],
                          int number_of_nodes);
void compute_n_opt_move(int number_of_segments,
                        int tour[],
                        const int* edges_to_remove);

#endif //MATH_UTIL_H
