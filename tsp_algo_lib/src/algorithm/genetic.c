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

#include "random.h"

typedef struct {
    int *genes;
    double *costs;
    int n;          // Number of nodes
    int stride;     // Actual size in memory (n + 1) to include closing node
    int pop_size;
} Population;

static void population_alloc(Population *pop, const int size, const int n) {
    pop->n = n;
    pop->stride = n + 1; // Extra space for closing node
    pop->pop_size = size;
    // Use calloc for safety (initializes to 0)
    pop->genes = calloc(size * pop->stride, sizeof(int));
    pop->costs = calloc(size, sizeof(double));
    check_alloc(pop->genes);
    check_alloc(pop->costs);
}

static void population_free(const Population *pop) {
    free(pop->genes);
    free(pop->costs);
}

static void population_copy_individual(const Population *dest, int dest_idx,
                                       const Population *src, const int src_idx) {
    memcpy(&dest->genes[dest_idx * dest->stride],
           &src->genes[src_idx * src->stride],
           dest->stride * sizeof(int));
    dest->costs[dest_idx] = src->costs[src_idx];
}

static void population_swap(Population *pop, int idx1, int idx2) {
    if (idx1 == idx2) return;

    double temp_cost = pop->costs[idx1];
    pop->costs[idx1] = pop->costs[idx2];
    pop->costs[idx2] = temp_cost;

    int *temp_genes = malloc(pop->stride * sizeof(int));
    check_alloc(temp_genes);

    memcpy(temp_genes, &pop->genes[idx1 * pop->stride], pop->stride * sizeof(int));
    memcpy(&pop->genes[idx1 * pop->stride], &pop->genes[idx2 * pop->stride], pop->stride * sizeof(int));
    memcpy(&pop->genes[idx2 * pop->stride], temp_genes, pop->stride * sizeof(int));

    free(temp_genes);
}

static void repair_child(int *child, int n, const double *costs) {
    int *visited = calloc(n, sizeof(int));
    check_alloc(visited);

    int *temp_tour = malloc(n * sizeof(int));
    check_alloc(temp_tour);

    int current_len = 0;

    // Phase 1: Keep valid and unique nodes
    for (int i = 0; i < n; i++) {
        const int node = child[i];
        if (node >= 0 && node < n) {
            if (!visited[node]) {
                visited[node] = 1;
                temp_tour[current_len++] = node;
            }
        }
    }

    // Phase 2: Identify missing nodes
    int *missing = malloc(n * sizeof(int));
    check_alloc(missing);

    int missing_count = 0;
    for (int i = 0; i < n; i++) {
        if (!visited[i])
            missing[missing_count++] = i;
    }

    // Phase 3: Greedy insertion of missing nodes
    for (int k = 0; k < missing_count; k++) {
        const int node_to_insert = missing[k];
        double best_delta = DBL_MAX;
        int best_pos = -1;

        if (current_len == 0) {
            temp_tour[0] = node_to_insert;
            current_len++;
            continue;
        }

        // Simulate closed cycle to find best position
        for (int i = 0; i < current_len; i++) {
            const int u = temp_tour[i];
            const int v = temp_tour[(i + 1) % current_len];
            const double delta = costs[u * n + node_to_insert] +
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

    // Crucial: Explicitly close the tour
    child[n] = child[0];

    free(missing);
    free(temp_tour);
    free(visited);
}

static void crossover_operator(const int *parent1, const int *parent2,
                               int *child, const int n, const double *costs_matrix,
                               const TimeLimiter *timer, const int cut_min, const int cut_max,
                               RandomState *rng) {
    const int range = cut_max - cut_min;
    const int ratio = cut_min + random_int(rng, 0, range);
    const int cut_point = n * ratio / 100;

    memcpy(child, parent1, cut_point * sizeof(int));
    memcpy(child + cut_point,
           parent2 + cut_point,
           (n - cut_point) * sizeof(int));

    repair_child(child, n, costs_matrix);
    two_opt(child, n, costs_matrix, *timer);
}

static void mutate(int *tour, const int n, RandomState *rng) {
    const int i = random_int(rng, 0, n - 1);
    const int j = random_int(rng, 0, n - 1);
    const int t = tour[i];
    tour[i] = tour[j];
    tour[j] = t;

    // Ensure closure is consistent if index 0 was swapped
    tour[n] = tour[0];
}

static int tournament_selection(const Population *pop, int k, RandomState *rng) {
    int best_idx = random_int(rng, 0, pop->pop_size - 1);
    double best_cost = pop->costs[best_idx];

    for (int i = 1; i < k; i++) {
        const int idx = random_int(rng, 0, pop->pop_size - 1);
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
    RandomState rng;
    random_init(&rng, cfg->seed);
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

    int grasp_count = (cfg->population_size * cfg->init_grasp_percent) / 100;

    if_verbose(VERBOSE_INFO,
               "GA: Initializing population (%d GRASP, %d random)\n",
               grasp_count, cfg->population_size - grasp_count);

    for (int i = 0; i < cfg->population_size; i++) {
        // Access individual using correct stride
        int *tour = &current_pop.genes[i * current_pop.stride];

        if (i < grasp_count) {
            double c;
            int res = grasp_nearest_neighbor_tour(random_int(&rng, 0, n - 1),
                                                  tour,
                                                  n,
                                                  costs_matrix, &c,
                                                  cfg->init_grasp_rcl_size,
                                                  cfg->init_grasp_prob,
                                                  &rng);
            // grasp_nearest_neighbor_tour already closes the tour (tour[n] = tour[0])

            if (res != 0) {
                // Fallback
                for (int k = 0; k < n; k++) tour[k] = k;
                tour[n] = tour[0];
            }
        } else {
            for (int k = 0; k < n; k++) tour[k] = k;
            // Fisher-Yates
            for (int k = n - 1; k > 0; k--) {
                const int j = random_int(&rng, 0, k);
                const int temp = tour[k];
                tour[k] = tour[j];
                tour[j] = temp;
            }
            tour[n] = tour[0]; // Explicitly close
        }

        two_opt(tour, n, costs_matrix, timer);
        current_pop.costs[i] = calculate_tour_cost(tour, n, costs_matrix);
    }

    int generation = 0;
    while (!time_limiter_is_over(&timer)) {
        if_verbose(VERBOSE_DEBUG, "GA: Generation %d\n", generation);

        // Elitism: move best individuals to the beginning
        for (int k = 0; k < cfg->elite_count && k < cfg->population_size; k++) {
            int best_idx = k;
            for (int i = k + 1; i < cfg->population_size; i++) {
                if (current_pop.costs[i] < current_pop.costs[best_idx]) {
                    best_idx = i;
                }
            }
            population_swap(&current_pop, k, best_idx);
            population_copy_individual(&next_pop, k, &current_pop, k);
        }

        double best_gen_cost = current_pop.costs[0];
        if_verbose(VERBOSE_INFO, "GA: Gen %d best=%.2f\n", generation, best_gen_cost);

        // Update solution (index 0 is the best after elitism swap)
        tsp_solution_update_if_better(solution,
                                      &current_pop.genes[0],
                                      best_gen_cost);

        cost_recorder_add(recorder, best_gen_cost);

        // Crossover and Mutation for the rest
        for (int i = cfg->elite_count; i < cfg->population_size; i++) {
            const int p1 = tournament_selection(&current_pop, cfg->tournament_size, &rng);
            const int p2 = tournament_selection(&current_pop, cfg->tournament_size, &rng);

            crossover_operator(
                &current_pop.genes[p1 * current_pop.stride],
                &current_pop.genes[p2 * current_pop.stride],
                &next_pop.genes[i * next_pop.stride],
                n, costs_matrix, &timer,
                cfg->crossover_cut_min_ratio,
                cfg->crossover_cut_max_ratio,
                &rng
            );

            if (random_double(&rng) < cfg->mutation_rate) {
                mutate(&next_pop.genes[i * next_pop.stride], n, &rng);
            }

            next_pop.costs[i] =
                    calculate_tour_cost(&next_pop.genes[i * next_pop.stride], n, costs_matrix);
        }

        const Population tmp = current_pop;
        current_pop = next_pop;
        next_pop = tmp;

        generation++;
    }

    if_verbose(VERBOSE_INFO, "GA: Time limit reached at gen %d\n", generation);

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