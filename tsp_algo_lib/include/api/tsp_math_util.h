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
 * @brief Generates a normalized random number.
 *
 * Produces a random number normalized to the range [0,1].
 *
 * @return A normalized random number.
 */
double normalized_rand();

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

/**
 * @brief Computes the cost change (delta) for a special n-opt move configuration
 *        where all segments are inverted except the first one.
 *
 * This function calculates the change in tour cost when performing an n-opt operation
 * with the specific pattern of inverting all segments except the first. The n-opt move
 * removes n edges from the tour and replaces them with n new edges.
 *
 * The algorithm works as follows:
 * 1. Calculates the cost of all edges that will be removed from the original tour
 * 2. Calculates the cost of all new edges that will be added to form the new tour
 * 3. Returns the difference (delta) between these costs
 *
 * @param number_of_segments Number of segments in the n-opt move (equals n)
 * @param tour Array representing the current tour configuration
 * @param edges_to_remove Array containing indices of the edges to be removed
 * @param edge_cost_array Matrix of edge costs between nodes (flattened 2D array)
 * @param number_of_nodes Total number of nodes in the problem
 *
 * @return The total cost change (delta) for the n-opt move. A negative value
 *         indicates an improvement in the tour cost.
 */
double compute_n_opt_cost(int number_of_segments,
                          int tour[],
                          const int *edges_to_remove,
                          const double edge_cost_array[],
                          int number_of_nodes);

/**
 * @brief Performs the actual n-opt move by inverting segments between removed edges
 *
 * This function implements the n-opt move by inverting (reversing) segments of the tour
 * between the edges that are to be removed. For each pair of consecutive edges to remove,
 * the function reverses the segment of the tour between them. This is part of the local
 * search algorithm for solving combinatorial optimization problems like the Traveling
 * Salesperson Problem (TSP).
 *
 * Note that the first edge in the original tour is removed as part of this operation,
 * which is important for understanding the resulting tour configuration.
 *
 * The function works in conjunction with compute_n_opt_cost which calculates the cost
 * change before the actual move is performed.
 *
 * @param number_of_edges_to_remove Number of edges to be removed in the n-opt move
 * @param tour Array representing the current tour configuration which will be modified
 * @param edges_to_remove Array containing indices of the edges to be removed
 * @param number_of_nodes
 */
void compute_n_opt_move(int number_of_edges_to_remove,
                        int tour[],
                        const int *edges_to_remove,
                        int number_of_nodes);

#endif //MATH_UTIL_H
