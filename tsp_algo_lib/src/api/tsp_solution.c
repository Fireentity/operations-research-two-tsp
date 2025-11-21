#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <c_util.h>
#include <pthread.h>
#include <stdbool.h>

#include "tsp_solution.h"

#include "constants.h"
#include "feasibility_result.h"
#include "tsp_algorithm.h"
#include "tsp_instance.h"
#include "tsp_math_util.h"
#include "logger.h" // Include the logger

struct TspSolutionState {
    double cost;
    int* const tour;
    const TspInstance* const instance;
    pthread_mutex_t mutex;
};

// --- Private Helper Functions ---

static double compute_cost(const TspSolution* solution) {
    const TspInstance* instance = solution->state->instance;
    return calculate_tour_cost(
        solution->state->tour,
        tsp_instance_get_num_nodes(instance),
        tsp_instance_get_cost_matrix(instance)
    );
}

// --- Public API Functions (Thread-Safe) ---

static double get_cost_safe(const TspSolution* solution) {
    pthread_mutex_lock(&solution->state->mutex);
    const double cost = solution->state->cost;
    pthread_mutex_unlock(&solution->state->mutex);
    return cost;
}

static void get_tour_copy_safe(const TspSolution* self, int* tour_buffer) {
    const int num_nodes = tsp_instance_get_num_nodes(self->state->instance);

    pthread_mutex_lock(&self->state->mutex);
    memcpy(tour_buffer, self->state->tour, (num_nodes + 1) * sizeof(int));
    pthread_mutex_unlock(&self->state->mutex);
}

static bool update_if_better_safe(const TspSolution* self, const int* new_tour, double new_cost) {
    bool updated = false;
    const int num_nodes = tsp_instance_get_num_nodes(self->state->instance);

    pthread_mutex_lock(&self->state->mutex);
    if (new_cost < self->state->cost - EPSILON) {
        if_verbose(VERBOSE_DEBUG, "    Solution: New best cost found: %lf (was: %lf). Updating.\n", new_cost,
                   self->state->cost);
        self->state->cost = new_cost;
        memcpy(self->state->tour, new_tour, (num_nodes + 1) * sizeof(int));
        updated = true;
    }
    pthread_mutex_unlock(&self->state->mutex);

    return updated;
}

static FeasibilityResult is_feasible_safe(const TspSolution* solution) {
    if_verbose(VERBOSE_DEBUG, "  Solution: Checking feasibility...\n");
    const TspInstance* instance = solution->state->instance;
    const int number_of_nodes = tsp_instance_get_num_nodes(instance);

    int tour_copy[number_of_nodes + 1];

    // Safely get a snapshot of the current state
    if_verbose(VERBOSE_DEBUG, "    Solution: Locking mutex to get snapshot.\n");
    pthread_mutex_lock(&solution->state->mutex);
    memcpy(tour_copy, solution->state->tour, (number_of_nodes + 1) * sizeof(int));
    const double cost_copy = solution->state->cost;
    pthread_mutex_unlock(&solution->state->mutex);

    int counter[number_of_nodes];
    memset(counter, 0, number_of_nodes * sizeof(counter[0]));

    for (int i = 0; i < number_of_nodes; i++) {
        if (tour_copy[i] < 0 || tour_copy[i] > number_of_nodes - 1) {
            if_verbose(VERBOSE_INFO, "  Solution: Feasibility FAILED: UNINITIALIZED_ENTRY (node %d is %d).\n", i,
                       tour_copy[i]);
            return UNINITIALIZED_ENTRY;
        }
        counter[tour_copy[i]]++;
        if (counter[tour_copy[i]] != 1) {
            if_verbose(VERBOSE_INFO, "  Solution: Feasibility FAILED: DUPLICATED_ENTRY (node %d).\n", tour_copy[i]);
            return DUPLICATED_ENTRY;
        }
    }

    const double calculated_cost = calculate_tour_cost(
        tour_copy,
        number_of_nodes,
        tsp_instance_get_cost_matrix(instance)
    );

    if (fabs(cost_copy - calculated_cost) > EPSILON) {
        if_verbose(VERBOSE_INFO, "  Solution: Feasibility FAILED: NON_MATCHING_COST (stored: %lf, calculated: %lf).\n",
                   cost_copy, calculated_cost);
        return NON_MATCHING_COST;
    }

    if_verbose(VERBOSE_DEBUG, "  Solution: Feasibility check PASSED.\n");
    return FEASIBLE;
}

static FeasibilityResult solve_safe(const TspSolution* solution, const TspAlgorithm* tsp_algorithm,
                                    const CostsPlotter* plotter) {
    if_verbose(VERBOSE_DEBUG, "  Solution: Handing off to algorithm->solve().\n");

    tsp_algorithm->solve(tsp_algorithm,
                         solution->state->instance,
                         solution,
                         plotter);

    if_verbose(VERBOSE_DEBUG, "  Solution: Algorithm->solve() finished. Running final feasibility check.\n");
    return is_feasible_safe(solution);
}

static void free_this(const TspSolution* solution) {
    if_verbose(VERBOSE_DEBUG, "Freeing TspSolution...\n");
    if (!solution) {
        if_verbose(VERBOSE_DEBUG, "  TspSolution is NULL, skipping free.\n");
        return;
    }
    if (solution->state) {
        if_verbose(VERBOSE_DEBUG, "  Destroying mutex.\n");
        pthread_mutex_destroy(&solution->state->mutex);
        if (solution->state->tour) {
            if_verbose(VERBOSE_DEBUG, "  Freeing tour array.\n");
            free(solution->state->tour);
        }
        if_verbose(VERBOSE_DEBUG, "  Freeing solution state.\n");
        free(solution->state);
    }
    if_verbose(VERBOSE_DEBUG, "  Freeing solution self.\n");
    free((void*)solution);
}

static int* init_tour(const int number_of_nodes) {
    if_verbose(VERBOSE_DEBUG, "    Solution: Allocating default tour array for %d nodes.\n", number_of_nodes);
    int* tour = calloc(number_of_nodes + 1, sizeof(int));
    check_alloc(tour);
    for (int i = 0; i < number_of_nodes; i++)
        tour[i] = i;
    tour[number_of_nodes] = tour[0];
    return tour;
}

// --- Public Constructor ---

TspSolution* init_solution(const TspInstance* instance) {
    if_verbose(VERBOSE_INFO, "Initializing TSP solution...\n");
    const int number_of_nodes = tsp_instance_get_num_nodes(instance);

    int* tour = init_tour(number_of_nodes);
    const double initial_cost = calculate_tour_cost(tour, number_of_nodes, tsp_instance_get_cost_matrix(instance));
    if_verbose(VERBOSE_DEBUG, "  Solution: Default tour cost calculated: %lf\n", initial_cost);

    const TspSolutionState state = {
        .cost = initial_cost,
        .tour = tour,
        .instance = instance,
        .mutex = 0
    };

    const TspSolution stack = {
        .free = free_this,
        .is_feasible = is_feasible_safe,
        .solve = solve_safe,
        .get_cost = get_cost_safe,
        .get_tour_copy = get_tour_copy_safe,
        .update_if_better = update_if_better_safe,
        .state = memdup(&state, sizeof(state))
    };

    if_verbose(VERBOSE_DEBUG, "  Solution: Finalizing solution object.\n");
    TspSolution* solution = memdup(&stack, sizeof(stack));
    pthread_mutex_init(&solution->state->mutex, NULL);
    return solution;
}
