#ifndef PLOT_UTIL_H
#define PLOT_UTIL_H

#include <tsp_instance.h>

/**
 * @brief Structure representing plotting boundaries.
 */
typedef struct {
    double min_x; /**< Minimum x-coordinate. */
    double max_x; /**< Maximum x-coordinate. */
    double min_y; /**< Minimum y-coordinate. */
    double max_y; /**< Maximum y-coordinate. */
} Bounds;

/**
 * @brief Plots a TSP tour.
 *
 * Generates a visual representation of the tour and saves it to an output file.
 *
 * @param tour Array representing the tour order.
 * @param number_of_nodes Number of nodes in the tour.
 * @param nodes Array of node coordinates.
 * @param output_name Name of the output file.
 */
void plot_tour(const int *tour, int number_of_nodes, const Node* nodes, const char *output_name);

/**
 * @brief Calculates the plotting boundaries for a TSP tour.
 *
 * Computes the minimum and maximum coordinates to determine the plot bounds.
 *
 * @param tour Array representing the tour order.
 * @param number_of_nodes Number of nodes in the tour.
 * @param nodes Array of node coordinates.
 * @return A Bounds structure containing the plot boundaries.
 */
Bounds calculate_plot_bounds(const int *tour, int number_of_nodes, const Node* nodes);

#endif //PLOT_UTIL_H
