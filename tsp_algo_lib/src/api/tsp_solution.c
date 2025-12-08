#include "tsp_solution.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "constants.h"
#include "feasibility_result.h"
#include "tsp_instance.h"
#include "tsp_math.h"
#include "c_util.h"
#include "logger.h"
#include "tsp_parser.h"
#include "tsp_parser_solution.h"

struct TspSolution {
    double cost;
    int *tour;
    const TspInstance *instance;
    pthread_mutex_t mutex;
};

static double compute_cost_internal(const TspInstance *instance, const int *tour) {
    return calculate_tour_cost(
        tour,
        tsp_instance_get_num_nodes(instance),
        tsp_instance_get_cost_matrix(instance)
    );
}

static int *allocate_tour(const int n) {
    int *tour = calloc(n + 1, sizeof(int));
    check_alloc(tour);
    return tour;
}

static void initialize_tour_identity(int *tour, const int n) {
    for (int i = 0; i < n; i++) {
        tour[i] = i;
    }
    tour[n] = 0;
}

TspSolution *tsp_solution_create(const TspInstance *instance) {
    if_verbose(VERBOSE_INFO, "Initializing TSP solution...\n");
    TspSolution *sol = malloc(sizeof(TspSolution));
    check_alloc(sol);

    int n = tsp_instance_get_num_nodes(instance);

    sol->instance = instance;
    sol->tour = allocate_tour(n);

    initialize_tour_identity(sol->tour, n);
    sol->cost = compute_cost_internal(instance, sol->tour);

    if (pthread_mutex_init(&sol->mutex, NULL) != 0) {
        free(sol->tour);
        free(sol);
        check_alloc(NULL);
    }

    if_verbose(VERBOSE_DEBUG, "  Solution: Default tour cost calculated: %lf\n", sol->cost);
    return sol;
}

TspSolution *tsp_solution_create_with_tour(const TspInstance *instance, const int *source_tour) {
    TspSolution *sol = tsp_solution_create(instance);
    const int n = tsp_instance_get_num_nodes(instance);

    memcpy(sol->tour, source_tour, (n + 1) * sizeof(int));
    sol->cost = compute_cost_internal(instance, sol->tour);

    return sol;
}

void tsp_solution_destroy(TspSolution *self) {
    if (!self) return;
    if_verbose(VERBOSE_DEBUG, "Freeing TspSolution...\n");

    pthread_mutex_destroy(&self->mutex);
    if (self->tour) {
        free(self->tour);
    }
    free(self);
}


FeasibilityResult tsp_solution_check_feasibility(TspSolution *self) {
    if_verbose(VERBOSE_DEBUG, "  Solution: Checking feasibility...\n");

    const int n = tsp_instance_get_num_nodes(self->instance);
    int *tour_copy = malloc((n + 1) * sizeof(int));
    check_alloc(tour_copy);

    pthread_mutex_lock(&self->mutex);
    memcpy(tour_copy, self->tour, (n + 1) * sizeof(int));
    const double cost_copy = self->cost;
    pthread_mutex_unlock(&self->mutex);

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
        double calculated_cost = compute_cost_internal(self->instance, tour_copy);
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

double tsp_solution_get_cost(TspSolution *self) {
    pthread_mutex_lock(&self->mutex);
    double c = self->cost;
    pthread_mutex_unlock(&self->mutex);
    return c;
}

void tsp_solution_get_tour(TspSolution *self, int *tour_buffer) {
    int n = tsp_instance_get_num_nodes(self->instance);
    pthread_mutex_lock(&self->mutex);
    memcpy(tour_buffer, self->tour, (n + 1) * sizeof(int));
    pthread_mutex_unlock(&self->mutex);
}

bool tsp_solution_update_if_better(TspSolution *self, const int *new_tour, double new_cost) {
    bool updated = false;
    int n = tsp_instance_get_num_nodes(self->instance);

    pthread_mutex_lock(&self->mutex);
    if (new_cost < self->cost - EPSILON) {
        if_verbose(VERBOSE_DEBUG, "\tSolution: New best cost found: %lf (was: %lf). Updating.\n", new_cost,
                   self->cost);
        self->cost = new_cost;
        memcpy(self->tour, new_tour, (n + 1) * sizeof(int));
        updated = true;
    }
    pthread_mutex_unlock(&self->mutex);

    return updated;
}

TspError tsp_solution_save(TspSolution *self, const char *path) {
    if (!self || !path) return TSP_ERR_MEMORY;

    FILE *f = fopen(path, "w");
    if (!f) return TSP_ERR_FILE_OPEN;

    pthread_mutex_lock(&self->mutex);

    int n = tsp_instance_get_num_nodes(self->instance);

    fprintf(f, "%s\n", SOL_V1_MAGIC);
    fprintf(f, "%s %.15g\n", SOL_V1_COST, self->cost);
    fprintf(f, "%s %d\n", SOL_V1_DIM, n);
    fprintf(f, "%s\n", SOL_V1_TOUR_SECTION);

    for (int i = 0; i <= n; i++) {
        fprintf(f, "%d ", self->tour[i]);
        if (i > 0 && i % 20 == 0 && i != n) fprintf(f, "\n");
    }
    fprintf(f, "\n%s\n", SOL_V1_EOF);

    pthread_mutex_unlock(&self->mutex);
    fclose(f);

    return TSP_OK;
}

TspError tsp_solution_load(TspSolution *self, const char *path) {
    if (!self || !path) return TSP_ERR_MEMORY;

    pthread_mutex_lock(&self->mutex);

    const int n = tsp_instance_get_num_nodes(self->instance);
    double loaded_cost = 0.0;

    const TspParserStatus st = tsp_parser_load_solution(path, n, self->tour, &loaded_cost);
    const TspError err = tsp_error_from_parser_status(st);

    if (err == TSP_OK) {
        self->cost = compute_cost_internal(self->instance, self->tour);
    }

    pthread_mutex_unlock(&self->mutex);
    return err;
}
