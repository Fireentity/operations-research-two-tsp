#ifndef MATH_UTIL_H
#define MATH_UTIL_H
#include "tsp_instance.h"

/**
 * @brief Calculates the total cost of a given tour.
 *
 * Computes the sum of edge costs based on the order of nodes in the tour.
 *
 * @param tour Array representing the order of nodes.
 * @param number_of_nodes Number of nodes in the tour.
 * @param edge_cost_array Array containing the cost for each edge.
 * @return Total cost of the tour.
 */
double calculate_tour_cost(const int *tour, int number_of_nodes, const double *edge_cost_array);

/**
 * @brief Initializes the edge cost array.
 *
 * Computes and returns an array containing the cost between nodes based on their coordinates.
 *
 * @param nodes Array of nodes.
 * @param number_of_nodes Number of nodes.
 * @return Pointer to the newly allocated edge cost array.
 */
double *init_edge_cost_array(const Node *nodes, int number_of_nodes);



#endif //MATH_UTIL_H