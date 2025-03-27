#include <constants.h>
#include <costs_plotter.h>
#include <c_util.h>
#include <nearest_neighbor.h>
#include <float.h>
#include <plot_util.h>
#include <stdlib.h>
#include <time_limiter.h>
#include "algorithms.h"

union TspExtendedAlgorithms {
    NearestNeighbor *nearest_neighbor;
};

static void free_this(const TspAlgorithm *self) {
    if (!self) return;
    if (self->extended) {
        if (self->extended->nearest_neighbor) free(self->extended->nearest_neighbor);
        free(self->extended);
    }
    free((void *) self);
}


static void solve(const TspAlgorithm *tsp_algorithm,
                  int tour[],
                  const int number_of_nodes,
                  const double edge_cost_array[],
                  double *cost) {
    // Start the timer
    const int time_limit = tsp_algorithm->extended->nearest_neighbor->time_limit;
    const TimeLimiter *time_limiter = init_time_limiter(time_limit);
    time_limiter->start(time_limiter);

    // Start the costs plotter
    const CostsPlotter* plotter = init_plotter(number_of_nodes);

    // Work on my own tour.
    int current_tour[number_of_nodes+1];
    int best_tour[number_of_nodes+1];
    double current_cost;

    // Initialize current_tour by copying the input tour.
    //TODO make it thread safe
    memcpy(current_tour, tour, (number_of_nodes + 1) * sizeof(int));

    // Prepare an array of starting nodes from the initial tour and shuffle them.
    int starting_nodes[number_of_nodes];
    memcpy(starting_nodes, tour, number_of_nodes * sizeof(int));
    shuffle_int_array(starting_nodes, number_of_nodes);

    // Initialize best_cost to a large value.
    double best_cost = DBL_MAX;

    int iteration = 0;
    // Main loop: try each starting node or run until the time limit expires.
    while (!time_limiter->is_time_over(time_limiter) && iteration < number_of_nodes) {
        // Build a NN solution starting from starting_nodes[iteration].
        nearest_neighbor_tour(starting_nodes[iteration], current_tour, number_of_nodes, edge_cost_array, &current_cost);
        // Improve the solution with 2-opt and update the cost.
        current_cost += two_opt(current_tour, number_of_nodes, edge_cost_array, time_limiter);
        plotter->add_cost(plotter, current_cost);
        // If the current solution is better than the best found so far, update best_tour and best_cost.
        if (current_cost < best_cost - EPSILON) {
            best_cost = current_cost;
            memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
        }
        iteration++;
    }

    // Copy the best found tour back to the provided output and update cost.
    // TODO make it thread safe
    if (best_cost < *cost) {
        memcpy(tour, best_tour, (number_of_nodes + 1) * sizeof(int));
        *cost = best_cost;
    }
    plotter->plot_costs(plotter, "NN-costs.png");
    // Cleanup.
    time_limiter->free(time_limiter);
    plotter->free(plotter);
}


const TspAlgorithm *init_nearest_neighbor(const double time_limit, const TspInstance *instance) {
    const NearestNeighbor nearest_neighbor = {
        .time_limit = time_limit,
        .instance = instance
    };

    const TspExtendedAlgorithms extended_algorithms = {
        .nearest_neighbor = malloc_from_stack(&nearest_neighbor, sizeof(nearest_neighbor))
    };

    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };

    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
