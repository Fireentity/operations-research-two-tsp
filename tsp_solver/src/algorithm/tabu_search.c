#include "tabu_search.h"
#include <constants.h>
#include <stdlib.h>
#include <time_limiter.h>
#include <tsp_math_util.h>
#include "c_util.h"


union TspExtendedAlgorithms {
    TabuSearch *tabu_search;
};

static void free_this(const TspAlgorithm *self) {
    if (!self) return;

    if (self->extended) {
        if (self->extended->tabu_search) {
            if (self->extended->tabu_search->tabu_list) free((void *) self->extended->tabu_search->tabu_list);
            free(self->extended->tabu_search);
        }
        free(self->extended);
    }
    free((void *) self);
}

// Evaluate a single neighbor using existing n-opt functions
static inline double evaluate_move(
    int tour[],
    int temp_tour[],
    const int i,
    const int j,
    const int number_of_nodes,
    const double edge_cost_array[]
) {
    // Copy current tour
    copy_int_array(tour, temp_tour, number_of_nodes + 1);

    // Prepare edges to remove for a 2-opt move
    const int edges_to_remove[] = {i, j};

    // Calculate cost delta
    return compute_n_opt_cost(2, temp_tour, edges_to_remove, edge_cost_array, number_of_nodes);
}

// Find the best non-tabu neighbor
static inline int find_best_neighbor(
    int tour[],
    int temp_tour[],
    const int number_of_nodes,
    const double edge_cost_array[],
    const int *tabu_list,
    const int tenure,
    double best_cost,
    int *best_i,
    int *best_j,
    double *best_delta,
    const TimeLimiter* time_limiter
) {
    *best_delta = 0;
    *best_i = -1;
    *best_j = -1;

    // Evaluate all possible 2-opt moves
    for (int i = 0; i < number_of_nodes - 1; i++) {
        if (time_limiter->is_time_over(time_limiter)) {
            return 0; // Time is over
        }

        for (int j = i + 2; j < number_of_nodes; j++) {
            // Skip if i == 0 and j == number_of_nodes - 1 (as in the original two_opt function)
            if (i == 0 && j == number_of_nodes - 1) continue;

            // Calculate move hash for tabu list
            const int move_hash = (i * number_of_nodes + j) % tenure;

            // Skip if move is in tabu list
            if (tabu_list[move_hash] != -1) {
                // Unless it satisfies the aspiration criterion (tested later)
                continue;
            }

            // Calculate cost delta using existing functions
            double delta = evaluate_move(tour, temp_tour, i, j, number_of_nodes, edge_cost_array);

            // If move doesn't improve, skip to next
            if (delta > -EPSILON) continue;

            // If it's the best delta so far, update
            if (delta < *best_delta) {
                *best_delta = delta;
                *best_i = i;
                *best_j = j;
            }
        }
    }

    // If no valid move was found
    if (*best_i == -1) return 0;

    return 1; // Valid move found
}

// Apply aspiration criterion to see if a tabu move can be accepted
static inline int check_aspiration_criterion(
    int tour[],
    int temp_tour[],
    const int number_of_nodes,
    const double edge_cost_array[],
    const int *tabu_list,
    const int tenure,
    const double best_cost,
    const double current_cost,
    int *best_i,
    int *best_j,
    double *best_delta,
    const TimeLimiter* time_limiter
) {
    // Evaluate all possible 2-opt moves that are in the tabu list
    for (int i = 0; i < number_of_nodes - 1; i++) {
        if (time_limiter->is_time_over(time_limiter)) {
            return 0; // Time is over
        }

        for (int j = i + 2; j < number_of_nodes; j++) {
            // Skip if i == 0 and j == number_of_nodes - 1
            if (i == 0 && j == number_of_nodes - 1) continue;

            // Calculate move hash for tabu list
            const int move_hash = (i * number_of_nodes + j) % tenure;

            // Consider only moves in the tabu list
            if (tabu_list[move_hash] == -1) continue;

            // Calculate cost delta
            const double delta = evaluate_move(tour, temp_tour, i, j, number_of_nodes, edge_cost_array);

            // If move doesn't improve, skip to next
            if (delta > -EPSILON) continue;

            // Check aspiration criterion: does the move improve the best solution found?
            if (current_cost + delta < best_cost) {
                *best_delta = delta;
                *best_i = i;
                *best_j = j;
                return 1; // Found a move that satisfies the aspiration criterion
            }
        }
    }

    return 0; // No move satisfies the aspiration criterion
}

// Main solve function restructured
static void solve(const TspAlgorithm *tsp_algorithm,
                  int tour[],
                  const int number_of_nodes,
                  const double edge_cost_array[],
                  double *cost) {
    // Initialize time limiter
    const int time_limit = tsp_algorithm->extended->tabu_search->time_limit;
    const TimeLimiter *time_limiter = init_time_limiter(time_limit);
    time_limiter->start(time_limiter);

    // Starts from random solution
    shuffle_int_array(tour, number_of_nodes);
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);

    // Best tour found
    double best_cost = *cost;
    int best_tour[number_of_nodes + 1];
    copy_int_array(tour, best_tour, number_of_nodes + 1);

    // Initialize tabu list
    int *tabu_list = (int *) tsp_algorithm->extended->tabu_search->tabu_list;
    const int tenure = tsp_algorithm->extended->tabu_search->tenure;

    // Clear tabu list
    for (int i = 0; i < tenure; i++) {
        tabu_list[i] = -1;
    }

    // Current tabu index (circular buffer)
    int tabu_index = 0;

    // Temp tour for evaluating neighbors

    // Number of iterations without improvement (for diversification)
    int stagnation_counter = 0;
    const int max_stagnation = number_of_nodes * 2;

    // Solve cycle
    while (!time_limiter->is_time_over(time_limiter)) {
        int temp_tour[number_of_nodes + 1];
        // Find best non-tabu neighbor
        double best_delta;
        int best_i, best_j;
        int found_move = find_best_neighbor(
            tour, temp_tour, number_of_nodes, edge_cost_array,
            tabu_list, tenure, best_cost, &best_i, &best_j, &best_delta, time_limiter
        );

        // If time is over, exit
        if (time_limiter->is_time_over(time_limiter)) {
            break;
        }

        // If no valid move was found, try with aspiration criterion
        if (!found_move) {
            found_move = check_aspiration_criterion(
                tour, temp_tour, number_of_nodes, edge_cost_array,
                tabu_list, tenure, best_cost, *cost, &best_i, &best_j, &best_delta, time_limiter
            );
        }

        // If still no valid move was found, diversify or terminate
        if (!found_move) {
            stagnation_counter++;

            // If too many iterations without improvement, diversify
            if (stagnation_counter >= max_stagnation) {
                shuffle_int_array(tour, number_of_nodes);
                *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
                stagnation_counter = 0;
            }

            continue;
        }

        // Apply the best move found using your compute_n_opt_move function
        const int edges_to_remove[] = {best_i, best_j};
        compute_n_opt_move(2, tour, edges_to_remove, number_of_nodes);

        // Update current cost
        *cost += best_delta;

        // Reset stagnation counter if there was an improvement
        stagnation_counter = 0;

        // Update the best solution if improved
        if (*cost < best_cost) {
            best_cost = *cost;
            copy_int_array(tour, best_tour, number_of_nodes + 1);
        }

        // Add move to tabu list
        const int move_hash = (best_i * number_of_nodes + best_j) % tenure;
        tabu_list[tabu_index] = move_hash;
        tabu_index = (tabu_index + 1) % tenure;
    }

    // Restore best tour found
    copy_int_array(best_tour, tour, number_of_nodes + 1);
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);

    time_limiter->free(time_limiter);
}

const TspAlgorithm *init_tabu(const int tenure, const double time_limit) {
    int *list = malloc(sizeof(int) * tenure);
    const TabuSearch tabu_search = {.tenure = tenure, .time_limit = time_limit, .tabu_list = list};
    const TspExtendedAlgorithms extended_algorithms = {
        .tabu_search = malloc_from_stack(&tabu_search, sizeof(tabu_search))
    };
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
