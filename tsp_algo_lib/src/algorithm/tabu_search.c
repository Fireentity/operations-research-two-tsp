#include "tabu_search.h"
#include <float.h>
#include <stdlib.h>
#include <string.h>
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
    TabuSearch* tabu_search;
};

static void free_this(const TspAlgorithm* self) {
    if (!self) return;
    if_verbose(VERBOSE_DEBUG, "Freeing Tabu Search algorithm struct...\n");
    if (self->extended) {
        if (self->extended->tabu_search) {
            free(self->extended->tabu_search);
        }
        free(self->extended);
    }
    free((void*)self);
}

static void improve(const TspAlgorithm* tsp_algorithm,
                    const TspInstance* instance,
                    const TspSolution* solution,
                    const CostsPlotter* plotter) {
    if_verbose(VERBOSE_DEBUG, "  TS: Starting improvement loop...\n");
    const int number_of_nodes = instance->get_number_of_nodes(instance);
    const double* edge_cost_array = instance->get_edge_cost_array(instance);

    const double time_limit = tsp_algorithm->extended->tabu_search->time_limit;
    const int tabu_tenure = tsp_algorithm->extended->tabu_search->tenure;
    const int max_stagnation = tsp_algorithm->extended->tabu_search->max_stagnation;

    if_verbose(VERBOSE_DEBUG, "  TS: Time limit=%.2fs, Tenure=%d, Max Stagnation=%d\n",
               time_limit, tabu_tenure, max_stagnation);

    const TimeLimiter* time_limiter = init_time_limiter(time_limit);
    time_limiter->start(time_limiter);

    int current_tour[number_of_nodes + 1];
    solution->get_tour_copy(solution, current_tour);
    double current_cost = solution->get_cost(solution);

    if_verbose(VERBOSE_DEBUG, "  TS: Running initial 2-Opt...\n");
    current_cost += two_opt(current_tour, number_of_nodes, edge_cost_array, time_limiter, EPSILON);

    int best_tour[number_of_nodes + 1];
    memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
    double best_cost = current_cost;
    if_verbose(VERBOSE_DEBUG, "  TS: Initial cost after 2-Opt: %lf\n", best_cost);

    int* tabu = calloc(number_of_nodes * number_of_nodes, sizeof(int));
    check_alloc(tabu);

    int no_improvements = 0;
    int iteration = 0;

    while (!time_limiter->is_time_over(time_limiter) && no_improvements < max_stagnation) {
        iteration++;
        int best_i = -1, best_j = -1;
        double best_delta = DBL_MAX;

        for (int i = 1; i < number_of_nodes - 1; i++) {
            for (int j = i + 1; j < number_of_nodes; j++) {
                const int a = current_tour[i - 1];
                const int b = current_tour[i];
                const int c = current_tour[j];
                const int d = current_tour[j + 1];

                const double delta = edge_cost_array[a * number_of_nodes + c] +
                    edge_cost_array[b * number_of_nodes + d] -
                    (edge_cost_array[a * number_of_nodes + b] +
                        edge_cost_array[c * number_of_nodes + d]);

                const int tabu_index = i * number_of_nodes + j;
                const int move_is_tabu = (tabu[tabu_index] > iteration);

                // Aspiration criterion
                if (move_is_tabu && (current_cost + delta) >= best_cost)
                    continue;

                if (delta < best_delta - EPSILON) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        if (best_i == -1 || best_j == -1) {
            if_verbose(VERBOSE_DEBUG, "  TS: No non-tabu moves found, terminating.\n");
            break;
        }

        const int edges[2] = {best_i - 1, best_j};
        compute_n_opt_move(2, current_tour, edges, number_of_nodes);
        current_cost += best_delta;

        tabu[best_i * number_of_nodes + best_j] = iteration + tabu_tenure;

        if (current_cost < best_cost - EPSILON) {
            if_verbose(VERBOSE_DEBUG, "    TS [Iteration %d]: Found new best cost: %lf (Stagnation reset)\n", iteration,
                       current_cost);
            best_cost = current_cost;
            memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
            no_improvements = 0;
        }
        else {
            no_improvements++;
        }
        plotter->add_cost(plotter, current_cost);
    }

    if (no_improvements >= max_stagnation) {
        if_verbose(VERBOSE_DEBUG, "  TS: Improvement loop stopped due to max stagnation (%d).\n", max_stagnation);
    }
    else if (time_limiter->is_time_over(time_limiter)) {
        if_verbose(VERBOSE_DEBUG, "  TS: Improvement loop stopped due to time limit.\n");
    }

    solution->update_if_better(solution, best_tour, best_cost);

    if_verbose(VERBOSE_DEBUG, "  TS: Cleaning up tabu list and time limiter.\n");
    free(tabu);
    time_limiter->free(time_limiter);
}

static void solve(const TspAlgorithm* tsp_algorithm,
                  const TspInstance* instance,
                  const TspSolution* solution,
                  const CostsPlotter* plotter) {
    if_verbose(VERBOSE_INFO, "Running Tabu Search algorithm...\n");
    const int number_of_nodes = instance->get_number_of_nodes(instance);
    const double* edge_cost_array = instance->get_edge_cost_array(instance);

    int initial_tour[number_of_nodes + 1];
    double initial_cost;
    solution->get_tour_copy(solution, initial_tour);

    if_verbose(VERBOSE_DEBUG, "  TS: Generating initial solution via Nearest Neighbor...\n");
    int result = nearest_neighbor_tour(rand() % number_of_nodes, initial_tour, number_of_nodes, edge_cost_array,
                                       &initial_cost);

    if (result != 0) {
        // Assuming 0 (ALGO_SUCCESS) is success
        if_verbose(VERBOSE_INFO, "  TS: Failed to generate initial solution.\n");
        return;
    }

    solution->update_if_better(solution, initial_tour, initial_cost);
    if_verbose(VERBOSE_DEBUG, "  TS: Initial solution cost: %lf\n", initial_cost);

    improve(tsp_algorithm, instance, solution, plotter);
    if_verbose(VERBOSE_INFO, "Tabu Search algorithm finished.\n");
}


const TspAlgorithm* init_tabu(const int tenure, const int max_stagnation, const double time_limit) {
    if_verbose(VERBOSE_DEBUG, "Initializing TS (tenure=%d, stagnation=%d, t=%.2f)\n", tenure, max_stagnation,
               time_limit);
    const TabuSearch tabu_search = {
        .tenure = tenure,
        .time_limit = time_limit,
        .max_stagnation = max_stagnation
    };
    const TspExtendedAlgorithms extended_algorithms = {
        .tabu_search = memdup(&tabu_search, sizeof(tabu_search))
    };
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended = memdup(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return memdup(&tsp_algorithm, sizeof(tsp_algorithm));
}
