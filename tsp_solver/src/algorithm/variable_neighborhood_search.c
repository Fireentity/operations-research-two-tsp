#include <constants.h>
#include <costs_plotter.h>
#include <variable_neighborhood_search.h>
#include <c_util.h>
#include <nearest_neighbor.h>
#include <time_limiter.h>
#include <tsp_math_util.h>
#include "algorithms.h"

union TspExtendedAlgorithms {
    VariableNeighborhoodSearch *variable_neighborhood_search;
};

/**
 * @brief Performs a kick move on the tour by applying an n-opt operation.
 *
 * This function randomly selects a set of non-contiguous edges to remove from the tour,
 * computes the resulting cost change for a specific n-opt move, updates the overall cost,
 * and then applies the move to modify the tour configuration.
 *
 * @param tour             Array representing the current tour.
 * @param number_of_nodes  Total number of nodes in the tour.
 * @param edge_cost_array  Matrix of edge costs (flattened 2D array).
 * @param n_opt            Number of opt
 */
static double kick(int tour[],
                   const int number_of_nodes,
                   const double *edge_cost_array,
                   const int n_opt) {
    // Array to store indices of edges to remove.
    int edges_to_remove[n_opt];

    // Compute the number of edges to remove.
    const int number_of_edges_to_remove = sizeof(edges_to_remove) / sizeof(edges_to_remove[0]);

    // Randomly select non-contiguous edges for removal.
    rand_k_non_contiguous(0, number_of_nodes - 1, number_of_edges_to_remove, edges_to_remove);

    // Compute the cost change of the n-opt move and update the total cost.
    const double delta = compute_n_opt_cost(n_opt, tour, edges_to_remove, edge_cost_array, number_of_nodes);

    // Apply the n-opt move to update the tour configuration.
    compute_n_opt_move(number_of_edges_to_remove, tour, edges_to_remove, number_of_nodes);

    return delta;
}

static void solve(const TspAlgorithm *tsp_algorithm,
                  int tour[],
                  const int number_of_nodes,
                  const double edge_cost_array[],
                  double *cost) {
    // Start the timer
    const int time_limit = tsp_algorithm->extended->variable_neighborhood_search->time_limit;
    const TimeLimiter *time_limiter = init_time_limiter(time_limit);
    time_limiter->start(time_limiter);

    // Start the costs plotter
    const CostsPlotter* plotter = init_plotter(number_of_nodes);

    // Work on my own tour
    int current_tour[number_of_nodes + 1];
    double current_cost;
    //TODO make it thread safe
    memcpy(current_tour, tour, (number_of_nodes + 1) * sizeof(int));

    // Start from a NN with 2-opt:
    nearest_neighbor_tour(rand() % number_of_nodes, current_tour, number_of_nodes, edge_cost_array, &current_cost);
    current_cost += two_opt(current_tour, number_of_nodes, edge_cost_array, time_limiter);

    // Save it as best tour
    int best_tour[number_of_nodes + 1];
    memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
    double best_cost = current_cost;


    const int kick_repetition = tsp_algorithm->extended->variable_neighborhood_search->kick_repetition;

    while (!time_limiter->is_time_over(time_limiter)) {
        for (int i = 0; i < kick_repetition; i++) {
            current_cost += kick(current_tour, number_of_nodes, edge_cost_array, 5);
        }
        current_cost += two_opt(current_tour, number_of_nodes, edge_cost_array, time_limiter);
        if (current_cost < best_cost - EPSILON) {
            memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
            best_cost = current_cost;
        }
        plotter->add_cost(plotter,current_cost);
    }

    // Copy the best found tour back to the provided output and update cost.
    // TODO make it thread safe
    if (best_cost < *cost) {
        memcpy(tour, best_tour, (number_of_nodes + 1) * sizeof(int));
        *cost = best_cost;
    }

    plotter->plot_costs(plotter, "VNS-costs.png");

    // Cleanup.
    time_limiter->free(time_limiter);
    plotter->free(plotter);
}

static void free_this(const TspAlgorithm *self) {
    if (!self) return;
    if (self->extended) {
        if (self->extended->variable_neighborhood_search) free(self->extended->variable_neighborhood_search);
        free(self->extended);
    }
    free((void *) self);
}

const TspAlgorithm *init_vns(const int kick_repetition, const double time_limit) {
    const VariableNeighborhoodSearch vns = {
        .kick_repetition = kick_repetition,
        .time_limit = time_limit,
    };
    const TspExtendedAlgorithms extended_algorithms = {
        .variable_neighborhood_search = malloc_from_stack(&vns, sizeof(vns))
    };
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
