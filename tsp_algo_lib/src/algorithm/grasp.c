#include "grasp.h"
#include "c_util.h"
#include <stdlib.h>
#include <string.h>
#include "nearest_neighbor.h"
#include "algorithms.h"
#include "constants.h"
#include "time_limiter.h"
#include "logger.h"
#include "tsp_instance.h"
#include "tsp_solution.h"

union TspExtendedAlgorithms {
    Grasp *grasp;
};

static void improve(const TspAlgorithm *tsp_algorithm,
                    const TspInstance *instance,
                    const TspSolution *solution,
                    const CostsPlotter *plotter) {
    if_verbose(VERBOSE_DEBUG, "  GRASP: Starting improvement loop...\n");
    const int number_of_nodes = tsp_instance_get_num_nodes(instance);
    const double *edge_cost_array = tsp_instance_get_cost_matrix(instance);

    const double time_limit = tsp_algorithm->extended->grasp->time_limit;
    if_verbose(VERBOSE_DEBUG, "  GRASP: Time limit set to %.2fs.\n", time_limit);
    TimeLimiter *time_limiter = time_limiter_create(time_limit);
    time_limiter_start(time_limiter);

    int current_tour[number_of_nodes + 1];
    int best_tour[number_of_nodes + 1];
    double current_cost;

    if_verbose(VERBOSE_DEBUG, "  GRASP: Getting initial tour for local improvement.\n");
    tsp_solution_get_tour(solution, current_tour);

    int starting_nodes[number_of_nodes];
    memcpy(starting_nodes, current_tour, number_of_nodes * sizeof(int));
    shuffle_int_array(starting_nodes, number_of_nodes);

    double best_cost = tsp_solution_get_cost(solution);
    memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
    if_verbose(VERBOSE_DEBUG, "  GRASP: Initial best cost: %lf\n", best_cost);

    int iteration = 0;
    while (!time_limiter_is_over(time_limiter) && iteration < number_of_nodes) {
        grasp_nearest_neighbor_tour(
            starting_nodes[iteration],
            current_tour,
            number_of_nodes,
            edge_cost_array,
            &current_cost,
            tsp_algorithm->extended->grasp->p1,
            tsp_algorithm->extended->grasp->p2,
            tsp_algorithm->extended->grasp->p3);

        current_cost += two_opt(current_tour, number_of_nodes, edge_cost_array, time_limiter, EPSILON);
        costs_plotter_add(plotter, current_cost);

        if (current_cost < best_cost - EPSILON) {
            best_cost = current_cost;
            memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
            if_verbose(VERBOSE_DEBUG, "    GRASP [Iteration %d]: Found new best cost: %lf\n", iteration, best_cost);
        }
        iteration++;
    }

    if_verbose(VERBOSE_DEBUG, "  GRASP: Improvement loop finished after %d iterations.\n", iteration);
    tsp_solution_update_if_better(solution, best_tour, best_cost);

    time_limiter_destroy(time_limiter);
}

static void solve(const TspAlgorithm *tsp_algorithm,
                  const TspInstance *instance,
                  const TspSolution *solution,
                  const CostsPlotter *plotter) {
    if_verbose(VERBOSE_INFO, "Running GRASP algorithm...\n");
    const int number_of_nodes = tsp_instance_get_num_nodes(instance);
    const double *edge_cost_array = tsp_instance_get_cost_matrix(instance);

    int initial_tour[number_of_nodes + 1];
    double initial_cost;
    tsp_solution_get_tour(solution, initial_tour);

    if_verbose(VERBOSE_DEBUG, "  GRASP: Generating initial solution...\n");
    const int result = grasp_nearest_neighbor_tour(
        rand() % number_of_nodes,
        initial_tour,
        number_of_nodes,
        edge_cost_array,
        &initial_cost,
        tsp_algorithm->extended->grasp->p1,
        tsp_algorithm->extended->grasp->p2,
        tsp_algorithm->extended->grasp->p3);

    if (result != 0) {
        if_verbose(VERBOSE_INFO, "  GRASP: Failed to generate initial solution.\n");
        return;
    }

    tsp_solution_update_if_better(solution, initial_tour, initial_cost);
    if_verbose(VERBOSE_DEBUG, "  GRASP: Initial solution cost: %lf\n", initial_cost);

    improve(tsp_algorithm, instance, solution, plotter);
    if_verbose(VERBOSE_INFO, "GRASP algorithm finished.\n");
}


static void free_this(const TspAlgorithm *self) {
    if (!self) return;

    if_verbose(VERBOSE_DEBUG, "Freeing GRASP algorithm struct...\n");
    if (self->extended) {
        if (self->extended->grasp) {
            free(self->extended->grasp);
        }
        free(self->extended);
    }
    free((void *) self);
}

const TspAlgorithm *init_grasp(const double time_limit, const double p1, const double p2) {
    if_verbose(VERBOSE_DEBUG, "Initializing GRASP (t=%.2f, p1=%.2f, p2=%.2f)\n", time_limit, p1, p2);
    const Grasp grasp = {
        .time_limit = time_limit,
        .p1 = p1,
        .p2 = p2,
        .p3 = 1 - p1 - p2
    };
    const TspExtendedAlgorithms extended_algorithms = {
        .grasp = memdup(&grasp, sizeof(grasp))
    };
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended = memdup(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return memdup(&tsp_algorithm, sizeof(tsp_algorithm));
}
