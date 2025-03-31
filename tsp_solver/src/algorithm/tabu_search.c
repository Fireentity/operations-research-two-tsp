#include "tabu_search.h"
#include <constants.h>
#include <costs_plotter.h>
#include <float.h>
#include <stdlib.h>
#include <time_limiter.h>
#include <tsp_math_util.h>

#include "c_util.h"
#include "algorithms.h"

// Union for extended TSP algorithms (currently only Tabu Search is used).
union TspExtendedAlgorithms {
    TabuSearch *tabu_search;
};


// Free function for TspAlgorithm: releases allocated memory.
static void free_this(const TspAlgorithm *self) {
    if (!self) return;
    if (self->extended) {
        if (self->extended->tabu_search) {
            free(self->extended->tabu_search);
        }
        free(self->extended);
    }
    free((void *) self);
}

// The improve function creates its own time limiter and cost plotter,
// then refines the tour using a tabu-search based 2‑opt procedure.
static void improve(const TspAlgorithm *tsp_algorithm,
                    int tour[],
                    const int number_of_nodes,
                    const double edge_cost_array[],
                    double *cost,
                    pthread_mutex_t *mutex) {
    // Initialize time limiter and cost plotter.
    const double time_limit = tsp_algorithm->extended->tabu_search->time_limit;
    const TimeLimiter *time_limiter = init_time_limiter(time_limit);
    time_limiter->start(time_limiter);
    const CostsPlotter *plotter = init_plotter(number_of_nodes);

    // Make a local copy of the tour and current cost.
    int current_tour[number_of_nodes + 1];
    double current_cost;
    WITH_MUTEX(mutex, memcpy(current_tour, tour, (number_of_nodes + 1) * sizeof(int));
               current_cost = *cost);

    current_cost += two_opt(tour, number_of_nodes, edge_cost_array, time_limiter);


    // Save the current tour as the best found solution so far.
    int best_tour[number_of_nodes + 1];
    memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
    double best_cost = current_cost;

    // Allocate the tabu list (flattened 2D array).
    int *tabu = calloc(number_of_nodes * number_of_nodes, sizeof(int));
    check_alloc(tabu);

    int no_improvements = 0;
    int iteration = 0;
    const int tabu_tenure = tsp_algorithm->extended->tabu_search->tenure;
    const int max_stagnation = tsp_algorithm->extended->tabu_search->max_stagnation;

    while (!time_limiter->is_time_over(time_limiter) && no_improvements < max_stagnation) {
        iteration++;
        int best_i = -1, best_j = -1;
        double best_delta = DBL_MAX; // Start with a large number.

        // Examine all possible 2‑opt moves.
        for (int i = 1; i < number_of_nodes - 1; i++) {
            for (int j = i + 1; j < number_of_nodes; j++) {
                // The four nodes involved in the 2‑opt move.
                const int a = current_tour[i - 1];
                const int b = current_tour[i];
                const int c = current_tour[j];
                const int d = current_tour[j + 1]; // current_tour has size number_of_nodes+1

                // Compute the change in cost (delta) for reversing the segment.
                const double delta = (edge_cost_array[a * number_of_nodes + c] +
                                      edge_cost_array[b * number_of_nodes + d]) -
                                     (edge_cost_array[a * number_of_nodes + b] +
                                      edge_cost_array[c * number_of_nodes + d]);

                // Check tabu status.
                const int tabu_index = i * number_of_nodes + j;
                const int move_is_tabu = (tabu[tabu_index] > iteration);

                // Aspiration criterion: allow tabu move if it yields a new best solution.
                if (move_is_tabu && (current_cost + delta) >= best_cost)
                    continue;

                // Select the move with the best (lowest) delta.
                if (delta < best_delta - EPSILON) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        // If no valid move was found, exit the loop.
        if (best_i == -1 || best_j == -1)
            break;

        // Apply the best 2‑opt move by reversing the segment from best_i to best_j.
        /*for (int k = 0; k < (best_j - best_i + 1) / 2; k++) {
            const int tmp = current_tour[best_i + k];
            current_tour[best_i + k] = current_tour[best_j - k];
            current_tour[best_j - k] = tmp;
        }*/
        const int edges[2] = {best_i-1,best_j};
        compute_n_opt_move(2,current_tour,edges, number_of_nodes);
        current_cost += best_delta;

        // Update tabu list: mark the move as forbidden until (iteration + tabu_tenure).
        tabu[best_i * number_of_nodes + best_j] = iteration + tabu_tenure;

        // If the new solution is an improvement, update the best solution.
        if (current_cost < best_cost - EPSILON) {
            best_cost = current_cost;
            memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
            no_improvements = 0;
        } else {
            no_improvements++;
        }
        plotter->add_cost(plotter, current_cost);
    }

    // If an improved solution was found, update the input tour and cost.
    if (best_cost < *cost) {
        WITH_MUTEX(mutex, memcpy(tour, best_tour, (number_of_nodes + 1) * sizeof(int));*cost = best_cost);
    }

    // Plot the cost progression.
    plotter->plot_costs(plotter, "TS-costs.png");

    // Cleanup resources.
    free(tabu);
    time_limiter->free(time_limiter);
    plotter->free(plotter);
}

// The refactored solve function creates an initial tour using the nearest neighbor heuristic
// and a 2‑opt improvement, then calls improve() to further optimize the solution.
static void solve(const TspAlgorithm *tsp_algorithm,
                  int tour[],
                  const int number_of_nodes,
                  const double edge_cost_array[],
                  double *cost,
                  pthread_mutex_t *mutex) {
    // Create the initial tour in a thread-safe manner.
    WITH_MUTEX(mutex,
               nearest_neighbor_tour(rand() % number_of_nodes, tour, number_of_nodes, edge_cost_array, cost);
    );
    // Improve the tour.
    improve(tsp_algorithm, tour, number_of_nodes, edge_cost_array, cost, mutex);
}


const TspAlgorithm *init_tabu(const int tenure, const int max_stagnation, const double time_limit) {
    // Create a TabuSearch structure with given parameters.
    const TabuSearch tabu_search = {
        .tenure = tenure,
        .time_limit = time_limit,
        .max_stagnation = max_stagnation
    };
    // Allocate and initialize extended algorithm structure with TabuSearch pointer.
    const TspExtendedAlgorithms extended_algorithms = {
        .tabu_search = malloc_from_stack(&tabu_search, sizeof(tabu_search))
    };
    // Initialize main TspAlgorithm structure with solve and free functions.
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .improve = improve,
        .free = free_this,
        .extended = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    // Return the allocated TspAlgorithm structure.
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
