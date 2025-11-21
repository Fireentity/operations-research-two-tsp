#include "tsp_solution.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#include "constants.h"
#include "feasibility_result.h"
#include "tsp_algorithm.h"
#include "tsp_instance.h"
#include "tsp_math_util.h"
#include "c_util.h"        // For check_alloc, memdup
#include "logger.h"

// Definition of the opaque struct (Flattened: no TspSolutionState)
struct TspSolution {
    double cost; // Current best cost
    int *tour; // Dynamic array size N+1
    const TspInstance *instance; // Reference to the instance
    pthread_mutex_t mutex; // Mutex for thread safety
};

// --- Internal Helper Functions ---

// Helper to calculate cost from a raw tour array
static double compute_cost_internal(const TspInstance *instance, const int *tour) {
    return calculate_tour_cost(
        tour,
        tsp_instance_get_num_nodes(instance),
        tsp_instance_get_cost_matrix(instance)
    );
}

// Helper to allocate tour memory
static int *allocate_tour(int n) {
    int *tour = calloc(n + 1, sizeof(int));
    check_alloc(tour);
    return tour;
}

// Helper to create identity tour (0->1->2->...->0)
static void initialize_tour_identity(int *tour, int n) {
    for (int i = 0; i < n; i++) {
        tour[i] = i;
    }
    tour[n] = 0; // Close the loop
}

// --- Constructors ---

TspSolution *tsp_solution_create(const TspInstance *instance) {
    if_verbose(VERBOSE_INFO, "Initializing TSP solution...\n");

    TspSolution *sol = malloc(sizeof(TspSolution));
    check_alloc(sol);

    int n = tsp_instance_get_num_nodes(instance);

    // Setup basic fields
    sol->instance = instance;
    sol->tour = allocate_tour(n);

    // Init default tour
    initialize_tour_identity(sol->tour, n);
    sol->cost = compute_cost_internal(instance, sol->tour);

    // Init Mutex
    if (pthread_mutex_init(&sol->mutex, NULL) != 0) {
        free(sol->tour);
        free(sol);
        check_alloc(NULL); // Force exit on mutex failure
    }

    if_verbose(VERBOSE_DEBUG, "  Solution: Default tour cost calculated: %lf\n", sol->cost);
    return sol;
}

TspSolution *tsp_solution_create_with_tour(const TspInstance *instance, const int *source_tour) {
    // Reuse base constructor
    TspSolution *sol = tsp_solution_create(instance);

    const int n = tsp_instance_get_num_nodes(instance);

    // Overwrite tour
    memcpy(sol->tour, source_tour, (n + 1) * sizeof(int));
    sol->cost = compute_cost_internal(instance, sol->tour);

    return sol;
}

// --- Destructor ---

void tsp_solution_destroy(TspSolution *self) {
    if_verbose(VERBOSE_DEBUG, "Freeing TspSolution...\n");
    if (!self) return;

    pthread_mutex_destroy(&self->mutex);

    if (self->tour) {
        free(self->tour);
    }

    free(self);
}

// --- Core Operations ---

FeasibilityResult tsp_solution_solve(TspSolution *self,
                                     const TspAlgorithm *tsp_algorithm,
                                     const CostsPlotter *plotter) {
    if_verbose(VERBOSE_DEBUG, "  Solution: Handing off to algorithm->solve().\n");

    // NOTE: Here we keep OOP style because TspAlgorithm is not refactored yet.
    // We pass 'self' because TspAlgorithm expects the opaque pointer.
    tsp_algorithm->solve(tsp_algorithm,
                         self->instance,
                         self,
                         plotter);

    if_verbose(VERBOSE_DEBUG, "  Solution: Algorithm finished. Running final feasibility check.\n");
    return tsp_solution_check_feasibility(self);
}

FeasibilityResult tsp_solution_check_feasibility(TspSolution *self) {
    if_verbose(VERBOSE_DEBUG, "  Solution: Checking feasibility...\n");

    int n = tsp_instance_get_num_nodes(self->instance);
    int *tour_copy = malloc((n + 1) * sizeof(int));
    check_alloc(tour_copy);
    double cost_copy;

    // Atomic snapshot
    pthread_mutex_lock(&self->mutex);
    memcpy(tour_copy, self->tour, (n + 1) * sizeof(int));
    cost_copy = self->cost;
    pthread_mutex_unlock(&self->mutex);

    // Validation
    int *counter = calloc(n, sizeof(int));
    check_alloc(counter);
    FeasibilityResult result = FEASIBLE;

    for (int i = 0; i < n; i++) {
        if (tour_copy[i] < 0 || tour_copy[i] >= n) {
            if_verbose(VERBOSE_INFO, "  Solution: Feasibility FAILED: UNINITIALIZED_ENTRY (node %d is %d).\n", i,
                       tour_copy[i]);
            result = UNINITIALIZED_ENTRY;
            break;
        }
        counter[tour_copy[i]]++;
        if (counter[tour_copy[i]] != 1) {
            if_verbose(VERBOSE_INFO, "  Solution: Feasibility FAILED: DUPLICATED_ENTRY (node %d).\n", tour_copy[i]);
            result = DUPLICATED_ENTRY;
            break;
        }
    }

    if (result == FEASIBLE) {
        const double calculated_cost = compute_cost_internal(self->instance, tour_copy);
        if (fabs(cost_copy - calculated_cost) > EPSILON) {
            if_verbose(VERBOSE_INFO,
                       "  Solution: Feasibility FAILED: NON_MATCHING_COST (stored: %lf, calculated: %lf).\n",
                       cost_copy, calculated_cost);
            result = NON_MATCHING_COST;
        }
    }

    if (result == FEASIBLE) {
        if_verbose(VERBOSE_DEBUG, "  Solution: Feasibility check PASSED.\n");
    }

    free(counter);
    free(tour_copy);
    return result;
}

// --- Accessors ---

double tsp_solution_get_cost(TspSolution *self) {
    pthread_mutex_lock(&self->mutex);
    double c = self->cost;
    pthread_mutex_unlock(&self->mutex);
    return c;
}

void tsp_solution_get_tour(TspSolution *self, int *tour_buffer) {
    const int n = tsp_instance_get_num_nodes(self->instance);

    pthread_mutex_lock(&self->mutex);
    memcpy(tour_buffer, self->tour, (n + 1) * sizeof(int));
    pthread_mutex_unlock(&self->mutex);
}

bool tsp_solution_update_if_better(TspSolution *self, const int *new_tour, double new_cost) {
    bool updated = false;
    const int n = tsp_instance_get_num_nodes(self->instance);

    pthread_mutex_lock(&self->mutex);
    if (new_cost < self->cost - EPSILON) {
        if_verbose(VERBOSE_DEBUG, "    Solution: New best cost found: %lf (was: %lf). Updating.\n", new_cost,
                   self->cost);
        self->cost = new_cost;
        memcpy(self->tour, new_tour, (n + 1) * sizeof(int));
        updated = true;
    }
    pthread_mutex_unlock(&self->mutex);

    return updated;
}
