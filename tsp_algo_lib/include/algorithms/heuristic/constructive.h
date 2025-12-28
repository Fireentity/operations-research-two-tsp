#ifndef CONSTRUCTIVE_H
#define CONSTRUCTIVE_H
#include "random.h"


/**
 * @brief Generates a Nearest Neighbor tour.
 */
int nearest_neighbor_tour(int starting_node,
                          int *tour,
                          int number_of_nodes,
                          const double *edge_cost_array,
                          double *cost);

/**
 * @brief GRASP Construction with RCL.
 */
int grasp_nearest_neighbor_tour(int starting_node,
                                int *tour,
                                int number_of_nodes,
                                const double *edge_cost_array,
                                double *cost,
                                int rcl_size,
                                double probability,
                                RandomState *rng);

/**
 * @brief Standard Extra Mileage Algorithm (starts from scratch).
 */
int extra_mileage_tour(int *tour,
                       int n,
                       const double *costs,
                       double *cost);

/**
 * @brief Completes a partial tour using the Extra Mileage (Cheapest Insertion) logic.
 * Used by both the standard EM algorithm and the Genetic Algorithm repair phase.
 *
 * @param tour Buffer containing the partial tour in the first 'current_count' positions.
 * @param current_count Number of nodes already in the tour.
 * @param n Total number of nodes.
 * @param costs Cost matrix.
 * @param visited Array of size n (1 if node is in partial tour, 0 otherwise).
 */
int extra_mileage_complete_tour(int *tour,
                                 int current_count,
                                 int n,
                                 const double *costs,
                                 int *visited);

#endif //CONSTRUCTIVE_H