#include "math_util.h"
#include "tsp_solution.h"
#include "constants.h"

double calculate_tour_cost(const unsigned long *const tour, const unsigned long number_of_nodes, const double *const edge_cost_array) {
    double cost = 0;
    for (int i = 0; i < tour_array_size(number_of_nodes); i++) {
        cost += edge_cost_array[tour[i] * number_of_nodes + tour[i + 1]];
    }
    return cost;
}

//TODO refactor without instance
Bounds calculate_plot_bounds(const unsigned long *tour, const TspInstance *instance) {
    Bounds b;
    const Node* nodes = get_nodes(instance);
    b.min_x = b.max_x = (double) nodes[tour[0]].x;
    b.min_y = b.max_y = (double) nodes[tour[0]].y;
    unsigned long n = get_number_of_nodes(instance);
    for (long i = 1; i < n; i++) {
        double x = (double) nodes[tour[i]].x;
        double y = (double) nodes[tour[i]].y;

        if (x < b.min_x) b.min_x = x;
        if (x > b.max_x) b.max_x = x;
        if (y < b.min_y) b.min_y = y;
        if (y > b.max_y) b.max_y = y;
    }
    b.min_x -= PLOT_PADDING;
    b.min_y -= PLOT_PADDING;
    b.max_x += PLOT_PADDING;
    b.max_y += PLOT_PADDING;
    return b;
}
