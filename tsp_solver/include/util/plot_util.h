#ifndef PLOT_UTIL_H
#define PLOT_UTIL_H

#include <tsp_instance.h>

typedef struct {
    double min_x, max_x;
    double min_y, max_y;
} Bounds;

void plot_tour(const unsigned long *tour, unsigned long number_of_nodes, const Node* nodes, const char *output_name);
Bounds calculate_plot_bounds(const unsigned long *tour, unsigned long number_of_nodes, const Node* nodes);

#endif //PLOT_UTIL_H
