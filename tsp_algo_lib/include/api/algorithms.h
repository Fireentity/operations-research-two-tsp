#ifndef ALGORITHMS_H
#define ALGORITHMS_H
#include "time_limiter.h"


/**
* @brief Performs a two-opt optimization on the tour.
 *
 * Iteratively examines pairs of edges to find a two-opt move that reduces the tour cost.
 * When a beneficial move is found, the tour is updated and the process restarts.
 *
 * @param tour            Array representing the current tour.
 * @param number_of_nodes Total number of nodes in the tour.
 * @param edge_cost_array Flattened 2D array containing edge costs.
 * @param time_limiter    Maximum allowed time for optimization, in seconds. If exceeded, the function terminates early.
 * @param epsilon
 *
 * @returns The cost improvement of the 2-opt application
 */
double two_opt(int *tour,
               int number_of_nodes,
               const double *edge_cost_array,
               const TimeLimiter *time_limiter,
               double epsilon);


int nearest_neighbor_tour(int starting_node,
                          int *tour,
                          int number_of_nodes,
                          const double *edge_cost_array,
                          double *cost);

int grasp_nearest_neighbor_tour(int starting_node,
                                int *tour,
                                int number_of_nodes,
                                const double *edge_cost_array,
                                double *cost,
                                double p1,
                                double p2,
                                double p3);

int grasp_nearest_neighbor_tour_threshold(int starting_node,
                                          int *tour,
                                          int number_of_nodes,
                                          const double *edge_cost_array,
                                          double *cost,
                                          double alpha);

#define MAX_NO_IMPROVEMENT_CYCLE 1000
#endif //ALGORITHMS_H