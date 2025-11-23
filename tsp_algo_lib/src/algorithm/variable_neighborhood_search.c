#include "variable_neighborhood_search.h"
#include <string.h>
#include <stdlib.h>
#include <c_util.h>
#include "algorithms.h"
#include "constants.h"
#include "costs_plotter.h"
#include "time_limiter.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "tsp_math_util.h"
#include "logger.h"


union TspExtendedAlgorithms {
    VariableNeighborhoodSearch *variable_neighborhood_search;
};

static double kick(int tour[],
                   const int number_of_nodes,
                   const double *edge_cost_array,
                   const int n_opt) {
    int edges_to_remove[n_opt];

    rand_k_non_contiguous(0, number_of_nodes - 1, n_opt, edges_to_remove);

    const double delta = compute_n_opt_cost(n_opt, tour, edges_to_remove, edge_cost_array, number_of_nodes);

    compute_n_opt_move(n_opt, tour, edges_to_remove, number_of_nodes);

    return delta;
}

static void improve(const TspAlgorithm *tsp_algorithm,
                    const TspInstance *instance,
                    TspSolution *solution,
                    CostsPlotter *plotter) {
    if_verbose(VERBOSE_DEBUG, "  VNS: Starting improvement loop...\n");
    const int number_of_nodes = tsp_instance_get_num_nodes(instance);
    const double *edge_cost_array = tsp_instance_get_cost_matrix(instance);

    const double time_limit = tsp_algorithm->extended->variable_neighborhood_search->time_limit;
    const int kick_repetition = tsp_algorithm->extended->variable_neighborhood_search->kick_repetition;
    const int n_opt = tsp_algorithm->extended->variable_neighborhood_search->n_opt;

    if_verbose(VERBOSE_DEBUG, "  VNS: Time limit=%.2fs, Kicks=%d, N-Opt=%d\n",
               time_limit, kick_repetition, n_opt);

    TimeLimiter *time_limiter = time_limiter_create(time_limit);
    time_limiter_start(time_limiter);

    int current_tour[number_of_nodes + 1];
    tsp_solution_get_tour(solution, current_tour);
    double current_cost = tsp_solution_get_cost(solution);

    if_verbose(VERBOSE_DEBUG, "  VNS: Running initial 2-Opt...\n");
    current_cost += two_opt(current_tour, number_of_nodes, edge_cost_array, time_limiter, EPSILON);

    int best_tour[number_of_nodes + 1];
    memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
    double best_cost = current_cost;
    if_verbose(VERBOSE_DEBUG, "  VNS: Initial cost after 2-Opt: %lf\n", best_cost);

    while (!time_limiter_is_over(time_limiter)) {
        // Kicking
        for (int i = 0; i < kick_repetition; i++) {
            current_cost += kick(current_tour, number_of_nodes, edge_cost_array, n_opt);
        }

        // Local Search (2-Opt)
        current_cost += two_opt(current_tour, number_of_nodes, edge_cost_array, time_limiter, EPSILON);

        if (current_cost < best_cost - EPSILON) {
            if_verbose(VERBOSE_DEBUG, "    VNS: Found new best cost: %lf\n", current_cost);
            memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
            best_cost = current_cost;
        }
        costs_plotter_add(plotter, current_cost);
    }

    if (time_limiter_is_over(time_limiter)) {
        if_verbose(VERBOSE_DEBUG, "  VNS: Improvement loop stopped due to time limit.\n");
    }

    tsp_solution_update_if_better(solution, best_tour, best_cost);

    if_verbose(VERBOSE_DEBUG, "  VNS: Cleaning up time limiter.\n");
    time_limiter_destroy(time_limiter);
}

static void solve(const TspAlgorithm *tsp_algorithm,
                  const TspInstance *instance,
                  TspSolution *solution,
                  CostsPlotter *plotter) {
    if_verbose(VERBOSE_INFO, "Running Variable Neighborhood Search algorithm...\n");
    const int number_of_nodes = tsp_instance_get_num_nodes(instance);
    const double *edge_cost_array = tsp_instance_get_cost_matrix(instance);

    int initial_tour[number_of_nodes + 1];
    double initial_cost;
    tsp_solution_get_tour(solution, initial_tour);

    if_verbose(VERBOSE_DEBUG, "  VNS: Generating initial solution via Nearest Neighbor...\n");
    const int result = nearest_neighbor_tour(rand() % number_of_nodes, initial_tour, number_of_nodes, edge_cost_array,
                                             &initial_cost);

    if (result != 0) {
        if_verbose(VERBOSE_INFO, "  VNS: Failed to generate initial solution.\n");
        return; // Exit if NN tour failed
    }

    tsp_solution_update_if_better(solution, initial_tour, initial_cost);
    if_verbose(VERBOSE_DEBUG, "  VNS: Initial solution cost: %lf\n", initial_cost);

    improve(tsp_algorithm, instance, solution, plotter);
    if_verbose(VERBOSE_INFO, "Variable Neighborhood Search algorithm finished.\n");
}


static void free_this(const TspAlgorithm *self) {
    if (!self) return;
    if_verbose(VERBOSE_DEBUG, "Freeing VNS algorithm struct...\n");
    if (self->extended) {
        if (self->extended->variable_neighborhood_search) free(self->extended->variable_neighborhood_search);
        free(self->extended);
    }
    free((void *) self);
}

const TspAlgorithm *init_vns(const int kick_repetition, const int n_opt, const double time_limit) {
    if_verbose(VERBOSE_DEBUG, "Initializing VNS (k=%d, n_opt=%d, t=%.2f)\n", kick_repetition, n_opt, time_limit);
    const VariableNeighborhoodSearch vns = {
        .kick_repetition = kick_repetition,
        .time_limit = time_limit,
        .n_opt = n_opt,
    };
    const TspExtendedAlgorithms extended_algorithms = {
        .variable_neighborhood_search = memdup(&vns, sizeof(vns))
    };
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended = memdup(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return memdup(&tsp_algorithm, sizeof(tsp_algorithm));
}
