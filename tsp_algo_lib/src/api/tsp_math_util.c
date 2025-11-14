#include <math.h>
#include <stdlib.h>
#include "c_util.h"
#include "tsp_instance.h"
#include "logger.h" // Include the logger

double calculate_tour_cost(const int* const tour,
                           const int number_of_nodes,
                           const double* const edge_cost_array) {
    double cost = 0;
    // --- NO LOGGING HERE ---
    // This function is performance-critical and called in tight loops.
    for (int i = 0; i < number_of_nodes; i++) {
        cost += edge_cost_array[tour[i] * number_of_nodes + tour[i + 1]];
    }
    return cost;
}

double normalized_rand() {
    return (double)rand() / RAND_MAX;
}

double* init_edge_cost_array(const Node* nodes, const int number_of_nodes) {
    if_verbose(VERBOSE_DEBUG, "    Allocating edge cost array for %d nodes (%lu bytes).\n",
               number_of_nodes, (unsigned long)(number_of_nodes * number_of_nodes * sizeof(double)));

    double* const edge_cost_array = calloc(number_of_nodes * number_of_nodes, sizeof(double));
    check_alloc(edge_cost_array);
    const double edge_array_size = number_of_nodes * number_of_nodes;

    if_verbose(VERBOSE_DEBUG, "    Calculating Euclidean distances for %d nodes...\n", number_of_nodes);
    for (int i = 0; i < edge_array_size; i++) {
        const int row = i / number_of_nodes;
        const int colum = i % number_of_nodes;
        if (row == colum) continue; // Cost is 0, already set by calloc

        const double dx = nodes[row].x - nodes[colum].x;
        const double dy = nodes[row].y - nodes[colum].y;
        edge_cost_array[i] = sqrt(dx * dx + dy * dy);
    }
    return edge_cost_array;
}


double compute_n_opt_cost(const int number_of_segments,
                          const int tour[],
                          const int* edges_to_remove,
                          const double edge_cost_array[],
                          const int number_of_nodes) {

    if_verbose(VERBOSE_ALL, "      Calculating %d-Opt cost delta...\n", number_of_segments);
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

    if_verbose(VERBOSE_ALL, "      %d-Opt delta calculated: %lf\n", number_of_segments, delta);
    return delta;
}


void compute_n_opt_move(const int number_of_edges_to_remove,
                        int tour[],
                        const int* edges_to_remove,
                        const int number_of_nodes) {

    if_verbose(VERBOSE_ALL, "      Performing %d-Opt move...\n", number_of_edges_to_remove);
    // This logic assumes edges_to_remove is sorted ascendingly
    for (int i = 0; i < number_of_edges_to_remove - 1; i++) {
        const int start_node = edges_to_remove[i] + 1;
        const int end_node = edges_to_remove[i + 1];

        if (start_node > end_node) {
            // This should not happen if the array is sorted
            if_verbose(VERBOSE_ALL, "[WARN] n_opt_move: edges_to_remove array is not sorted. Skipping reversal.\n");
            continue;
        }

        if_verbose(VERBOSE_ALL, "        Reversing segment [%d, %d]\n", start_node, end_node);
        reverse_array_int(tour, start_node, end_node);
    }
    tour[number_of_nodes] = tour[0];
}