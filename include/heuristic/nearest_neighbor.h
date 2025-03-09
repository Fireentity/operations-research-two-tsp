#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include "math_util.h"

/**
 * Solves the Traveling Salesman Problem (TSP) using the Nearest Neighbor heuristic.
 * This algorithm constructs a tour by always visiting the nearest unvisited node.
 *
 * @param tour Pointer to the array that will store the computed tour.
 * @param number_of_nodes The total number of nodes in the instance.
 * @param edge_cost_array Pointer to the array containing edge costs.
 * @param cost Pointer to store the total cost of the computed tour.
 */
void solve_with_nearest_neighbor(
        unsigned long *tour,
        unsigned long number_of_nodes,
        const double *edge_cost_array,
        double *cost
);

void solve_with_nearest_neighbor_and_two_opt(
        unsigned long *tour,
        unsigned long number_of_nodes,
        const double *edge_cost_array,
        double *cost
);

/**
 * Optimizes an existing TSP tour using the 2-opt algorithm.
 * The 2-opt algorithm improves a given tour by iteratively reversing segments
 * to remove crossing edges and reduce the total cost.
 *
 * @param tour Pointer to the array representing the current tour.
 * @param edge_cost_array Pointer to the array containing edge costs.
 * @param number_of_nodes The total number of nodes in the instance.
 * @param cost Pointer to store the updated cost of the optimized tour.
 */
void two_opt(unsigned long *tour, unsigned long number_of_nodes, const double *edge_cost_array, double *cost);

/**
 * Reverses a segment of the tour between indices i and k (inclusive).
 * This function is a helper for the 2-opt optimization.
 *
 * @param tour Pointer to the array representing the tour.
 * @param i Start index of the segment to reverse.
 * @param k End index of the segment to reverse.
 */
static void reverse_segment(unsigned long *tour, unsigned long i, unsigned long k);

#endif // NEAREST_NEIGHBOR_H
