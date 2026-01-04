#include "variable_neighborhood_search.h"
#include "tsp_math.h"
#include "time_limiter.h"
#include "c_util.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "local_search.h"
#include "random.h"
#include "tsp_tour.h"

static double vns_kick(int *tour, int n, const double *costs, int k_opt, RandomState *rng) {
    if (k_opt > n) k_opt = n;

    int *edges = tsp_malloc(k_opt * sizeof(int));

    const int low = 0;
    const int m = n - k_opt + 1;
    int s_prev = 0;

    int bound = m - (k_opt - 1);
    if (bound < 1) bound = 1;

    int s_curr = random_int(rng, 0, bound - 1);
    edges[0] = low + s_curr;
    s_prev = s_curr;

    for (int i = 1; i < k_opt; i++) {
        bound = m - s_prev - (k_opt - i);
        if (bound < 1) bound = 1;

        s_curr = s_prev + 1 + random_int(rng, 0, bound - 1);
        edges[i] = low + s_curr + i;
        s_prev = s_curr;
    }

    const double delta = compute_n_opt_cost(k_opt, tour, edges, costs, n);
    compute_n_opt_move(k_opt, tour, edges, n);

    tsp_free(edges);
    return delta;
}

static void run_vns(const TspInstance *instance,
                    TspSolution *solution,
                    const void *config_void,
                    CostRecorder *recorder) {
    const VNSConfig *cfg = config_void;
    RandomState rng;
    random_init(&rng, cfg->seed);
    const int n = tsp_instance_get_num_nodes(instance);
    const double *costs = tsp_instance_get_cost_matrix(instance);

    if_verbose(VERBOSE_INFO,
               "VNS: k=[%d..%d], kicks=%d, stagnation=%d, time=%.2f\n",
               cfg->min_k, cfg->max_k, cfg->kick_repetition,
               cfg->max_stagnation, cfg->time_limit);

    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    int *current_tour = tsp_malloc((n + 1) * sizeof(int));

    tsp_solution_get_tour(solution, current_tour);
    double current_cost = tsp_solution_get_cost(solution);

    // Initial descent strengthens the starting point
    current_cost += two_opt(current_tour, n, costs, timer);

    int *best_tour = memdup(current_tour, (n + 1) * sizeof(int));
    double best_cost = current_cost;

    int current_k = cfg->min_k;
    int stagnation = 0;

    while (!time_limiter_is_over(&timer) && stagnation < cfg->max_stagnation) {
        for (int i = 0; i < cfg->kick_repetition; i++)
            current_cost += vns_kick(current_tour, n, costs, current_k, &rng);

        current_cost += two_opt(current_tour, n, costs, timer);

        if (current_cost < best_cost - EPSILON) {
            if_verbose(VERBOSE_DEBUG,
                       "\tVNS: new global best %.2f at k=%d\n",
                       current_cost, current_k);

            best_cost = current_cost;
            memcpy(best_tour, current_tour, (n + 1) * sizeof(int));
            current_k = cfg->min_k;
            stagnation = 0;
        } else {
            memcpy(current_tour, best_tour, (n + 1) * sizeof(int));
            current_cost = best_cost;

            current_k++;
            if (current_k > cfg->max_k) {
                current_k = cfg->min_k;
                stagnation++;
            }
        }

        cost_recorder_add(recorder, current_cost);
    }

    if (time_limiter_is_over(&timer))
        if_verbose(VERBOSE_INFO, "\tVNS: time is over\n");

    if (stagnation >= cfg->max_stagnation)
        if_verbose(VERBOSE_INFO, "\tVNS: max stagnation reached\n");

    tsp_solution_update_if_better(solution, best_tour, best_cost);

    tsp_free(best_tour);
    tsp_free(current_tour);
}

static void *vns_clone_config(const void *config, uint64_t seed_offset) {
    const VNSConfig *src = config;
    VNSConfig *dest = tsp_malloc(sizeof(VNSConfig));
    *dest = *src;
    dest->seed += seed_offset;
    return dest;
}

static void free_vns_config(void *config) {
    tsp_free(config);
}

TspAlgorithm vns_create(const VNSConfig config) {
    VNSConfig *cfg_copy = tsp_malloc(sizeof(VNSConfig));

    *cfg_copy = config;

    return (TspAlgorithm){
        .name = "VNS",
        .config = cfg_copy,
        .run = run_vns,
        .free_config = free_vns_config,
        .clone_config = vns_clone_config,
    };
}
