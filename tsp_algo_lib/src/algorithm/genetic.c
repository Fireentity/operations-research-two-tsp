#include "genetic.h"
#include "tsp_math.h"
#include "constructive.h"
#include "local_search.h"
#include "c_util.h"
#include "logger.h"
#include "time_limiter.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>

typedef struct {
    int *genes;
    double *costs;
    int n;
    int pop_size;
} Population;

static void population_alloc(Population *pop, int size, int n) {
    pop->n = n;
    pop->pop_size = size;
    pop->genes = malloc(size * n * sizeof(int));
    pop->costs = malloc(size * sizeof(double));
    check_alloc(pop->genes);
    check_alloc(pop->costs);
}

static void population_free(Population *pop) {
    free(pop->genes);
    free(pop->costs);
}

static void population_copy_individual(Population *dest, int dest_idx,
                                       const Population *src, int src_idx) {
    memcpy(&dest->genes[dest_idx * dest->n],
           &src->genes[src_idx * src->n],
           dest->n * sizeof(int));
    dest->costs[dest_idx] = src->costs[src_idx];
}

/* Repair Strategy: Shortcut + Cheapest Insertion (Extra Mileage logic) */
static void repair_child(int *child, int n, const double *costs) {
    if_verbose(VERBOSE_DEBUG, "GA: Repairing child\n");

    int *visited = calloc(n, sizeof(int));
    check_alloc(visited);

    int *temp_tour = malloc(n * sizeof(int));
    check_alloc(temp_tour);

    int current_len = 0;
    for (int i = 0; i < n; i++) {
        int node = child[i];
        if (!visited[node]) {
            visited[node] = 1;
            temp_tour[current_len++] = node;
        }
    }

    int *missing = malloc((n - current_len) * sizeof(int));
    int missing_count = 0;
    for (int i = 0; i < n; i++) {
        if (!visited[i])
            missing[missing_count++] = i;
    }

    for (int k = 0; k < missing_count; k++) {
        int node_to_insert = missing[k];
        double best_delta = DBL_MAX;
        int best_pos = -1;

        for (int i = 0; i < current_len; i++) {
            int u = temp_tour[i];
            int v = temp_tour[(i + 1) % current_len];
            double delta = costs[u * n + node_to_insert] +
                           costs[node_to_insert * n + v] -
                           costs[u * n + v];
            if (delta < best_delta) {
                best_delta = delta;
                best_pos = i;
            }
        }

        for (int j = current_len; j > best_pos + 1; j--)
            temp_tour[j] = temp_tour[j - 1];

        temp_tour[best_pos + 1] = node_to_insert;
        current_len++;
    }

    memcpy(child, temp_tour, n * sizeof(int));

    free(missing);
    free(temp_tour);
    free(visited);
}

static void crossover_operator(const int *parent1, const int *parent2,
                               int *child, int n, const double *costs_matrix,
                               TimeLimiter *timer, int cut_min, int cut_max) {
    int range = cut_max - cut_min;
    int ratio = cut_min + (rand() % (range + 1));
    int cut_point = (n * ratio) / 100;

    if_verbose(VERBOSE_DEBUG,
               "GA: Crossover cut=%d (ratio=%d)\n", cut_point, ratio);

    memcpy(child, parent1, cut_point * sizeof(int));
    memcpy(child + cut_point,
           parent2 + cut_point,
           (n - cut_point) * sizeof(int));

    repair_child(child, n, costs_matrix);

    if_verbose(VERBOSE_DEBUG, "GA: Running 2-opt on child\n");
    two_opt(child, n, costs_matrix, *timer);
}

static void mutate(int *tour, int n) {
    int i = rand() % n;
    int j = rand() % n;
    int t = tour[i];
    tour[i] = tour[j];
    tour[j] = t;
}

static int tournament_selection(const Population *pop, int k) {
    int best_idx = rand() % pop->pop_size;
    double best_cost = pop->costs[best_idx];

    for (int i = 1; i < k; i++) {
        int idx = rand() % pop->pop_size;
        if (pop->costs[idx] < best_cost) {
            best_idx = idx;
            best_cost = pop->costs[idx];
        }
    }
    return best_idx;
}

static void run_genetic(const TspInstance *instance,
                        TspSolution *solution,
                        const void *config_void,
                        CostRecorder *recorder) {
    const GeneticConfig *cfg = config_void;
    const int n = tsp_instance_get_num_nodes(instance);
    const double *costs_matrix = tsp_instance_get_cost_matrix(instance);

    if_verbose(VERBOSE_INFO,
               "Genetic: Pop=%d, Elite=%d, Mut=%.2f, Time=%.2f\n",
               cfg->population_size,
               cfg->elite_count,
               cfg->mutation_rate,
               cfg->time_limit);

    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    Population current_pop, next_pop;
    population_alloc(&current_pop, cfg->population_size, n);
    population_alloc(&next_pop, cfg->population_size, n);

    int grasp_count = (cfg->population_size * 90) / 100;

    if_verbose(VERBOSE_INFO,
               "GA: Initializing population (%d GRASP, %d random)\n",
               grasp_count, cfg->population_size - grasp_count);

    for (int i = 0; i < cfg->population_size; i++) {
        int *tour = &current_pop.genes[i * n];

        if (i < grasp_count) {
            if_verbose(VERBOSE_DEBUG, "GA: Init individual %d via GRASP\n", i);
            double c;
            grasp_nearest_neighbor_tour((int) (normalized_rand() * n), tour, n, costs_matrix, &c, 5, 0.2);
        } else {
            if_verbose(VERBOSE_DEBUG, "GA: Init individual %d via random\n", i);
            for (int k = 0; k < n; k++) tour[k] = k;
            shuffle_int_array(tour, n);
        }

        two_opt(tour, n, costs_matrix, timer);
        current_pop.costs[i] = calculate_tour_cost(tour, n, costs_matrix);

        if_verbose(VERBOSE_DEBUG,
                   "GA: Init indiv %d cost=%.3f\n", i, current_pop.costs[i]);
    }

    int generation = 0;
    while (!time_limiter_is_over(&timer)) {
        if_verbose(VERBOSE_DEBUG,
                   "GA: Generation %d\n", generation);

        int best_idx = 0;
        for (int i = 1; i < cfg->population_size; i++)
            if (current_pop.costs[i] < current_pop.costs[best_idx])
                best_idx = i;

        if_verbose(VERBOSE_INFO,
                   "GA: Gen %d best=%.2f\n",
                   generation, current_pop.costs[best_idx]);

        tsp_solution_update_if_better(solution,
                                      &current_pop.genes[best_idx * n],
                                      current_pop.costs[best_idx]);

        cost_recorder_add(recorder, current_pop.costs[best_idx]);

        population_copy_individual(&next_pop, 0, &current_pop, best_idx);

        for (int i = 1; i < cfg->population_size; i++) {
            int p1 = tournament_selection(&current_pop, 5);
            int p2 = tournament_selection(&current_pop, 5);

            if_verbose(VERBOSE_DEBUG,
                       "GA: Crossover parents: %d & %d → child %d\n",
                       p1, p2, i);

            crossover_operator(
                &current_pop.genes[p1 * n],
                &current_pop.genes[p2 * n],
                &next_pop.genes[i * n],
                n, costs_matrix, &timer,
                cfg->crossover_cut_min_ratio,
                cfg->crossover_cut_max_ratio
            );

            if (normalized_rand() < cfg->mutation_rate) {
                if_verbose(VERBOSE_DEBUG,
                           "GA: Mutation applied to indiv %d\n", i);
                mutate(&next_pop.genes[i * n], n);
            }

            next_pop.costs[i] =
                    calculate_tour_cost(&next_pop.genes[i * n], n, costs_matrix);
        }

        Population tmp = current_pop;
        current_pop = next_pop;
        next_pop = tmp;

        generation++;
    }

    if_verbose(VERBOSE_INFO,
               "GA: Time limit reached at gen %d\n", generation);

    population_free(&current_pop);
    population_free(&next_pop);
}

static void free_genetic_config(void *config) {
    free(config);
}

TspAlgorithm genetic_create(GeneticConfig config) {
    GeneticConfig *cfg_copy = malloc(sizeof(GeneticConfig));
    check_alloc(cfg_copy);
    *cfg_copy = config;

    return (TspAlgorithm){
        .name = "Genetic Algorithm",
        .config = cfg_copy,
        .run = run_genetic,
        .free_config = free_genetic_config
    };
}
