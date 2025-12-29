#ifndef PLOT_UTIL_H
#define PLOT_UTIL_H
#include <stddef.h>

#include "tsp_instance.h"


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
void plot_tour(const int *tour, int number_of_nodes, const Node *nodes, const char *output_name);

/**
 * @brief Plots the evolution of costs over time.
 *
 * @param costs Array of cost values.
 * @param length Number of points.
 * @param output_name Output image file name.
 */
void plot_costs_evolution(const double *costs, size_t length, const char *output_name);
#endif //PLOT_UTIL_H
