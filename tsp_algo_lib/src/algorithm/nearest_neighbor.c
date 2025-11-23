#include "nearest_neighbor.h"
#include <stdlib.h>
#include <string.h>
#include <c_util.h>
#include "algorithms.h"
#include "constants.h"
#include "costs_plotter.h"
#include "time_limiter.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "logger.h"

union TspExtendedAlgorithms {
    NearestNeighbor *nearest_neighbor;
};

static void free_this(const TspAlgorithm *self) {
    if (!self) return;
    if_verbose(VERBOSE_DEBUG, "Freeing NN algorithm struct...\n");
    if (self->extended) {
        if (self->extended->nearest_neighbor) free(self->extended->nearest_neighbor);
        free(self->extended);
    }
    free((void *) self);
}

static void improve(const TspAlgorithm *tsp_algorithm,
                    const TspInstance *instance,
                    TspSolution *solution,
                    CostsPlotter *plotter) {
    if_verbose(VERBOSE_DEBUG, "  NN: Starting improvement loop (multi-start NN + 2-Opt)...\n");
    const int number_of_nodes = tsp_instance_get_num_nodes(instance);
    const double *edge_cost_array = tsp_instance_get_cost_matrix(instance);

    const double time_limit = tsp_algorithm->extended->nearest_neighbor->time_limit;
    if_verbose(VERBOSE_DEBUG, "  NN: Time limit set to %.2fs.\n", time_limit);
    TimeLimiter *time_limiter = time_limiter_create(time_limit);
    time_limiter_start(time_limiter);

    int current_tour[number_of_nodes + 1];
    int best_tour[number_of_nodes + 1];
    double current_cost;

    tsp_solution_get_tour(solution, current_tour);

    int starting_nodes[number_of_nodes];
    memcpy(starting_nodes, current_tour, number_of_nodes * sizeof(int));
    shuffle_int_array(starting_nodes, number_of_nodes);

    double best_cost = tsp_solution_get_cost(solution);
    memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
    if_verbose(VERBOSE_DEBUG, "  NN: Initial best cost: %lf\n", best_cost);

    int iteration = 0;
    while (!time_limiter_is_over(time_limiter) && iteration < number_of_nodes) {
        if_verbose(VERBOSE_ALL, "    NN [Iteration %d]: Running from start node %d.\n", iteration,
                   starting_nodes[iteration]);

        const int result = nearest_neighbor_tour(starting_nodes[iteration], current_tour, number_of_nodes,
                                                 edge_cost_array, &current_cost);
        if (result != 0) {
            if_verbose(VERBOSE_INFO, "    NN [Iteration %d]: Failed to generate NN tour.\n", iteration);
            iteration++;
            continue;
        }

        current_cost += two_opt(current_tour, number_of_nodes, edge_cost_array, time_limiter, EPSILON);
        costs_plotter_add(plotter, current_cost);

        if (current_cost < best_cost - EPSILON) {
            best_cost = current_cost;
            memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
            if_verbose(VERBOSE_DEBUG, "    NN [Iteration %d]: Found new best cost: %lf\n", iteration, best_cost);
        }
        iteration++;
    }

    if_verbose(VERBOSE_DEBUG, "  NN: Improvement loop finished after %d iterations.\n", iteration);
    tsp_solution_update_if_better(solution, best_tour, best_cost);

    time_limiter_destroy(time_limiter);
}

static void solve(const TspAlgorithm *tsp_algorithm,
                  const TspInstance *instance,
                  TspSolution *solution,
                  CostsPlotter *plotter) {
    if_verbose(VERBOSE_INFO, "Running Nearest Neighbor algorithm...\n");
    const int number_of_nodes = tsp_instance_get_num_nodes(instance);
    const double *edge_cost_array = tsp_instance_get_cost_matrix(instance);

    int initial_tour[number_of_nodes + 1];
    double initial_cost;

    if_verbose(VERBOSE_DEBUG, "  NN: Generating initial solution...\n");
    const int result = nearest_neighbor_tour(rand() % number_of_nodes, initial_tour, number_of_nodes, edge_cost_array,
                                             &initial_cost);

    if (result != 0) {
        if_verbose(VERBOSE_INFO, "  NN: Failed to generate initial solution.\n");
        return;
    }

    tsp_solution_update_if_better(solution, initial_tour, initial_cost);
    if_verbose(VERBOSE_DEBUG, "  NN: Initial solution cost: %lf\n", initial_cost);

    improve(tsp_algorithm, instance, solution, plotter);
    if_verbose(VERBOSE_INFO, "Nearest Neighbor algorithm finished.\n");
}


const TspAlgorithm *init_nearest_neighbor(const double time_limit) {
    if_verbose(VERBOSE_DEBUG, "Initializing NN (t=%.2f)\n", time_limit);
    const NearestNeighbor nearest_neighbor = {
        .time_limit = time_limit,
    };

    const TspExtendedAlgorithms extended_algorithms = {
        .nearest_neighbor = memdup(&nearest_neighbor, sizeof(nearest_neighbor))
    };

    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended = memdup(&extended_algorithms, sizeof(extended_algorithms)),
    };

    return memdup(&tsp_algorithm, sizeof(tsp_algorithm));
}
