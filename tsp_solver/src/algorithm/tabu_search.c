#include "tabu_search.h"
#include <constants.h>
#include <stdlib.h>
#include <time_limiter.h>
#include <tsp_math_util.h>
#include "c_util.h"
#include "hashmap.h"
#include "algorithms.h"

// Structure holding the indices of a move (2-opt edge reversal).
typedef struct {
    int i, j;
} TabuKey;

// Structure for a tabu entry with its expiration iteration.
typedef struct {
    TabuKey key;
    int expiration;
} TabuEntry;

// Hash function for TabuEntry: combines indices i and j.
static uint64_t tabu_entry_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const TabuEntry *e = item;
    uint64_t h = e->key.i; // Start with index i.
    h = h * 31 + (uint64_t) e->key.j; // Combine with index j.
    return h;
}

// Compare function for TabuEntry: compares i values, then j values.
static int tabu_entry_compare(const void *a, const void *b, void *udata) {
    const TabuEntry *ea = a, *eb = b;
    return (ea->key.i != eb->key.i) ? ea->key.i - eb->key.i : ea->key.j - eb->key.j;
}

// Union for extended TSP algorithms (currently only Tabu Search is used).
union TspExtendedAlgorithms {
    TabuSearch *tabu_search;
};

// Free function for TspAlgorithm: releases allocated memory.
static void free_this(const TspAlgorithm *self) {
    if (!self) return; // If self is NULL, exit.
    if (self->extended) {
        // Check if extended field is set.
        if (self->extended->tabu_search) {
            // Check if tabu_search exists.
            if (self->extended->tabu_search->tabu_map)
                hashmap_free(self->extended->tabu_search->tabu_map); // Free tabu map.
            free(self->extended->tabu_search); // Free tabu_search structure.
        }
        free(self->extended); // Free extended algorithms structure.
    }
    free((void *) self); // Free main TspAlgorithm structure.
}

// Evaluates a 2-opt move by computing the cost delta when reversing the segment between indices i and j.
static double evaluate_move(
    int tour[],
    int temp_tour[],
    const int i,
    const int j,
    const int number_of_nodes,
    const double edge_cost_array[]
) {
    copy_int_array(tour, temp_tour, number_of_nodes + 1); // Copy current tour to temp_tour.
    const int edges_to_remove[] = {i, j}; // Specify edges to remove.
    return compute_n_opt_cost(2, temp_tour, edges_to_remove, edge_cost_array, number_of_nodes); // Return cost delta.
}

// Checks if the move (i, j) is tabu at the current iteration.
// Returns 1 if tabu, 0 otherwise.
static int is_tabu(struct hashmap *tabu_map, const int i, const int j, const int iter) {
    const TabuEntry probe = {{i, j}, 0}; // Create a probe entry.
    const TabuEntry *entry = hashmap_get(tabu_map, &probe); // Lookup in tabu_map.
    if (entry) {
        // If an entry exists:
        if (entry->expiration > iter) // If not expired, move is tabu.
            return 1;
        hashmap_delete(tabu_map, &probe); // Delete expired entry.
    }
    return 0; // Move is not tabu.
}

// Searches for the best non-tabu 2-opt move.
// Iterates over all valid pairs (i, j) and updates best_i, best_j, and best_delta if a better move is found.
// Returns true if a valid move is found.
static bool find_best_neighbor(
    int tour[],
    int temp_tour[],
    const int number_of_nodes,
    const double edge_cost_array[],
    struct hashmap *tabu_map,
    const int iter,
    int *best_i,
    int *best_j,
    double *best_delta,
    const TimeLimiter *time_limiter
) {
    *best_delta = 0; // Initialize best_delta.
    *best_i = -1; // Initialize best_i.
    *best_j = -1; // Initialize best_j.
    for (int i = 0; i < number_of_nodes - 1; i++) {
        // Loop over start index i.
        if (time_limiter->is_time_over(time_limiter)) // Check time limit.
            return 0;
        for (int j = i + 2; j < number_of_nodes; j++) {
            // Loop over end index j ensuring non-adjacency.
            if (i == 0 && j == number_of_nodes - 1) // Skip move reversing entire tour.
                continue;
            if (is_tabu(tabu_map, i, j, iter)) // Skip if move is tabu.
                continue;
            const double delta = evaluate_move(tour, temp_tour, i, j, number_of_nodes, edge_cost_array);
            // Compute cost delta.
            if (delta > -EPSILON) // If move doesn't improve, skip.
                continue;
            if (delta < *best_delta) {
                // If move is better than current best:
                *best_delta = delta; // Update best_delta.
                *best_i = i; // Update best_i.
                *best_j = j; // Update best_j.
            }
        }
    }
    return *best_i != -1; // Return true if a valid move was found.
}

// Checks the aspiration criterion: even if a move is tabu, accept it if it yields a better solution than best_cost.
// Returns true if such a move is found.
static bool check_aspiration_criterion(
    int tour[],
    int temp_tour[],
    const int number_of_nodes,
    const double edge_cost_array[],
    struct hashmap *tabu_map,
    const int iter,
    const double best_cost,
    const double current_cost,
    int *best_i,
    int *best_j,
    double *best_delta,
    const TimeLimiter *time_limiter
) {
    for (int i = 0; i < number_of_nodes - 1; i++) {
        // Loop over start index i.
        if (time_limiter->is_time_over(time_limiter)) // Check time limit.
            return 0;
        for (int j = i + 2; j < number_of_nodes; j++) {
            // Loop over end index j.
            if (i == 0 && j == number_of_nodes - 1) // Skip full tour reversal.
                continue;
            if (!is_tabu(tabu_map, i, j, iter)) // Only consider moves that are tabu.
                continue;
            const double delta = evaluate_move(tour, temp_tour, i, j, number_of_nodes, edge_cost_array);
            // Compute cost delta.
            if (delta > -EPSILON) // If move doesn't improve, skip.
                continue;
            if (current_cost + delta < best_cost) {
                // If move yields a better solution:
                *best_delta = delta; // Update best_delta.
                *best_i = i; // Update best_i.
                *best_j = j; // Update best_j.
                return true; // Accept the move.
            }
        }
    }
    return false; // No tabu move meets the aspiration criterion.
}

// Main solve function for Tabu Search.
// First applies a 2-opt optimization to the initial tour, then performs the Tabu Search.
static void solve(const TspAlgorithm *tsp_algorithm,
                  int tour[],
                  const int number_of_nodes,
                  const double edge_cost_array[],
                  double *cost) {
    const double time_limit = tsp_algorithm->extended->tabu_search->time_limit; // Retrieve time limit.
    const TimeLimiter *time_limiter = init_time_limiter(time_limit); // Initialize time limiter.
    time_limiter->start(time_limiter); // Start time limiter.

    // Create initial random solution.
    shuffle_int_array(tour, number_of_nodes);
    // Apply 2-opt on the initial tour for local optimization.
    two_opt(tour, number_of_nodes, edge_cost_array, time_limiter);
    // Calculate cost of the improved initial tour.
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
    double best_cost = *cost; // Set best_cost to current cost.
    int best_tour[number_of_nodes + 1]; // Array to store best tour.
    copy_int_array(tour, best_tour, number_of_nodes + 1); // Copy current tour to best_tour.

    struct hashmap *tabu_map = tsp_algorithm->extended->tabu_search->tabu_map; // Get tabu map.
    int iter = 0, stagnation = 0; // Initialize iteration and stagnation counters.
    const int max_stagnation = tsp_algorithm->extended->tabu_search->max_stagnation; // Maximum allowed stagnation.

    // Main loop: iterate until time limit is exceeded.
    while (!time_limiter->is_time_over(time_limiter)) {
        int temp_tour[number_of_nodes + 1]; // Temporary tour for move evaluation.
        double best_delta; // Best cost improvement found.
        int best_i, best_j; // Indices of best move.
        // Attempt to find the best non-tabu neighbor move.
        bool move_found = find_best_neighbor(tour, temp_tour, number_of_nodes,
                                             edge_cost_array, tabu_map, iter,
                                             &best_i, &best_j, &best_delta, time_limiter);
        if (time_limiter->is_time_over(time_limiter)) // Check time limit after neighbor search.
            break;
        // If no non-tabu move was found, try aspiration criterion.
        if (!move_found)
            move_found = check_aspiration_criterion(tour, temp_tour, number_of_nodes,
                                                    edge_cost_array, tabu_map, iter,
                                                    best_cost, *cost,
                                                    &best_i, &best_j, &best_delta, time_limiter);
        // If still no valid move, increase stagnation.
        if (!move_found) {
            stagnation++; // Increment stagnation counter.
            if (stagnation >= max_stagnation) {
                // If stagnation limit reached:
                shuffle_int_array(tour, number_of_nodes); // Diversify by shuffling tour.
                *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array); // Recalculate cost.
                stagnation = 0; // Reset stagnation counter.
            }
            iter++; // Increment iteration counter.
            continue; // Continue to next iteration.
        }
        // Apply the best move by performing a 2-opt reversal between best_i and best_j.
        const int edges_to_remove[] = {best_i, best_j};
        compute_n_opt_move(2, tour, edges_to_remove, number_of_nodes);
        *cost += best_delta; // Update cost with improvement.
        stagnation = 0; // Reset stagnation as move was made.
        if (*cost < best_cost) {
            // If new cost is better than best_cost:
            best_cost = *cost; // Update best_cost.
            copy_int_array(tour, best_tour, number_of_nodes + 1); // Save current tour as best_tour.
        }
        // Add the move to the tabu map with expiration set to current iteration + tenure.
        TabuEntry entry = {{best_i, best_j}, iter + tsp_algorithm->extended->tabu_search->tenure};
        hashmap_set(tabu_map, &entry);
        iter++; // Increment iteration counter.
    }
    copy_int_array(best_tour, tour, number_of_nodes + 1); // Restore best tour found.
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array); // Recalculate final cost.
    time_limiter->free(time_limiter); // Free time limiter resources.
}

// Initializes the Tabu Search algorithm with specified parameters and returns a pointer to TspAlgorithm.
const TspAlgorithm *init_tabu(const int tenure, const int max_stagnation, const double time_limit) {
    // Create a new tabu map with capacity 'tenure' and random seeds.
    struct hashmap *tabu_map = hashmap_new(sizeof(TabuEntry), tenure, rand(), rand(),
                                           tabu_entry_hash, tabu_entry_compare, NULL, NULL);
    // Create a TabuSearch structure with given parameters.
    const TabuSearch tabu_search = {
        .tenure = tenure,
        .tabu_map = tabu_map,
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
