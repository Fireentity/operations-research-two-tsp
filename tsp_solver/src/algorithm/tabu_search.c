#include "tabu_search.h"
#include <constants.h>
#include <costs_plotter.h>
#include <float.h>
#include <stdlib.h>
#include <time_limiter.h>
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

// Solve the tsp problem without any starting tour
static void solve(const TspAlgorithm *tsp_algorithm,
                  int tour[],
                  const int number_of_nodes,
                  const double edge_cost_array[],
                  double *cost) {
    // Start time limiter.
    const double time_limit = tsp_algorithm->extended->tabu_search->time_limit;
    const TimeLimiter *time_limiter = init_time_limiter(time_limit);
    time_limiter->start(time_limiter);

    // Start the costs plotter
    const CostsPlotter *plotter = init_plotter(number_of_nodes);

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

    // Tabu list
    int *tabu = calloc(number_of_nodes * number_of_nodes, sizeof(int));
    check_alloc(tabu);


    int no_improvements = 0;
    int iteration = 0;
    const int tabu_tenure = tsp_algorithm->extended->tabu_search->tenure;
    const int max_stagnation = tsp_algorithm->extended->tabu_search->max_stagnation;

    while (!time_limiter->is_time_over(time_limiter) && no_improvements < max_stagnation) {
        iteration++;
        int best_i = -1, best_j = -1;
        double best_delta = DBL_MAX; // Use a large number as the initial best delta.

        // Examine all possible 2-opt moves.
        // Loop indices: i from 1 to number_of_nodes-2, j from i+1 to number_of_nodes-1.
        for (int i = 1; i < number_of_nodes - 1; i++) {
            for (int j = i + 1; j < number_of_nodes; j++) {
                // Identify the nodes involved in the potential move.
                const int a = current_tour[i - 1];
                const int b = current_tour[i];
                const int c = current_tour[j];
                const int d = current_tour[j + 1]; // j+1 is valid since current_tour has size number_of_nodes+1

                // Compute the change in cost (delta) for the 2-opt move.
                const double delta = (edge_cost_array[a * number_of_nodes + c] +
                                      edge_cost_array[b * number_of_nodes + d]) -
                                     (edge_cost_array[a * number_of_nodes + b] +
                                      edge_cost_array[c * number_of_nodes + d]);

                // Check tabu status from our flattened array.
                const int tabu_index = i * number_of_nodes + j;
                const int move_is_tabu = (tabu[tabu_index] > iteration);

                // Aspiration criterion: allow tabu move if it yields a solution better than best found.
                if (move_is_tabu && (current_cost + delta) >= best_cost)
                    continue;

                // Choose the move with the best (lowest) delta.
                if (delta < best_delta - EPSILON) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        // If no valid move was found, break out.
        if (best_i == -1 || best_j == -1)
            break;

        // Apply the best 2-opt move by reversing the segment from best_i to best_j.
        for (int k = 0; k < (best_j - best_i + 1) / 2; k++) {
            const int tmp = current_tour[best_i + k];
            current_tour[best_i + k] = current_tour[best_j - k];
            current_tour[best_j - k] = tmp;
        }

        current_cost += best_delta;

        // Update the tabu list: mark the move as forbidden until (iteration + tabu_tenure).
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
    // Copy the best found tour back to the provided output and update cost.
    // TODO make it thread safe
    if (best_cost < *cost) {
        memcpy(tour, best_tour, (number_of_nodes + 1) * sizeof(int));
        *cost = best_cost;
    }

    plotter->plot_costs(plotter, "TS-costs.png");

    // Cleanup.
    free(tabu);
    time_limiter->free(time_limiter);
    plotter->free(plotter);
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
        .free = free_this,
        .extended = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    // Return the allocated TspAlgorithm structure.
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
