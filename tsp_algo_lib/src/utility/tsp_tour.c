#include "tsp_tour.h"
#include "c_util.h"
#include "logger.h"


double compute_n_opt_cost(int number_of_segments,
                          int tour[],
                          const int *edges_to_remove,
                          const double edge_cost_array[],
                          int number_of_nodes) {
    if_verbose(VERBOSE_ALL, "\tCalculating %d-Opt cost delta...\n", number_of_segments);
    double delta = 0.0;
    int u, v; // To store node IDs

    // Subtract the cost of all edges that will be removed from the tour
    for (int i = 0; i < number_of_segments; i++) {
        const int start_node_idx = edges_to_remove[i];
        const int end_node_idx = start_node_idx + 1;
        u = tour[start_node_idx];
        v = tour[end_node_idx];
        delta -= edge_cost_array[u * number_of_nodes + v];
    }

    int start_node_idx = 0;
    int end_node_idx = 0;

    // Add the cost of new connections between middle segments
    for (int i = 1; i < number_of_segments - 1; i++) {
        start_node_idx = edges_to_remove[i - 1] + 1;
        end_node_idx = edges_to_remove[(i + 1) % number_of_segments];
        u = tour[start_node_idx];
        v = tour[end_node_idx];
        delta += edge_cost_array[u * number_of_nodes + v];
    }

    // Add the cost of connecting the first segment (not inverted) to the second segment
    start_node_idx = edges_to_remove[0];
    end_node_idx = edges_to_remove[1];
    u = tour[start_node_idx];
    v = tour[end_node_idx];
    delta += edge_cost_array[u * number_of_nodes + v];

    // Add the cost of connecting the last segment back to the first segment
    start_node_idx = edges_to_remove[number_of_segments - 2] + 1;
    end_node_idx = edges_to_remove[number_of_segments - 1] + 1;
    u = tour[start_node_idx];
    v = tour[end_node_idx];
    delta += edge_cost_array[u * number_of_nodes + v];

    if_verbose(VERBOSE_ALL, "\t\t%d-Opt delta calculated: %lf\n", number_of_segments, delta);
    return delta;
}


void compute_n_opt_move(const int number_of_edges_to_remove,
                        int tour[],
                        const int *edges_to_remove,
                        const int number_of_nodes) {
    if_verbose(VERBOSE_ALL, "\tPerforming %d-Opt move...\n", number_of_edges_to_remove);
    // This logic assumes edges_to_remove is sorted ascendingly
    for (int i = 0; i < number_of_edges_to_remove - 1; i++) {
        const int start_node = edges_to_remove[i] + 1;
        const int end_node = edges_to_remove[i + 1];

        if (start_node > end_node) {
            // This should not happen if the array is sorted
            if_verbose(VERBOSE_ALL, "[WARN] n_opt_move: edges_to_remove array is not sorted. Skipping reversal.\n");
            continue;
        }

        if_verbose(VERBOSE_ALL, "\t\tReversing segment [%d, %d]\n", start_node, end_node);
        reverse_array_int(tour, start_node, end_node);
    }
    tour[number_of_nodes] = tour[0];
}
