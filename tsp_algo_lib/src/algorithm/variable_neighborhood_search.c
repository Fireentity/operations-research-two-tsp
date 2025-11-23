#include "variable_neighborhood_search.h"
#include "algorithms.h"
#include "tsp_math_util.h"
#include "time_limiter.h"
#include "c_util.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>

#include "constants.h"

static double vns_kick(int *tour, int n, const double *costs, int n_opt) {
    int *edges = malloc(n_opt * sizeof(int));
    check_alloc(edges);

    rand_k_non_contiguous(0, n - 1, n_opt, edges);

    const double delta = compute_n_opt_cost(n_opt, tour, edges, costs, n);
    compute_n_opt_move(n_opt, tour, edges, n);

    free(edges);
    return delta;
}

static void run_vns(const TspInstance *instance,
                    TspSolution *solution,
                    const void *config_void,
                    CostRecorder *recorder) {
    const VNSConfig *cfg = config_void;
    const int n = tsp_instance_get_num_nodes(instance);
    const double *costs = tsp_instance_get_cost_matrix(instance);

    if_verbose(VERBOSE_DEBUG, "  VNS (k=%d, n_opt=%d, t=%.2f)\n",
               cfg->kick_repetition, cfg->n_opt, cfg->time_limit);

    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    int *current_tour = malloc((n + 1) * sizeof(int));
    check_alloc(current_tour);

    tsp_solution_get_tour(solution, current_tour);
    double current_cost = tsp_solution_get_cost(solution);

    current_cost += two_opt(current_tour, n, costs, timer, );

    int *best_tour = malloc((n + 1) * sizeof(int));
    check_alloc(best_tour);
    memcpy(best_tour, current_tour, (n + 1) * sizeof(int));
    double best_cost = current_cost;

    while (!time_limiter_is_over(&timer)) {
        for (int i = 0; i < cfg->kick_repetition; i++) {
            current_cost += vns_kick(current_tour, n, costs, cfg->n_opt);
        }

        current_cost += two_opt(current_tour, n, costs, timer, );

        if (current_cost < best_cost - EPSILON) {
            if_verbose(VERBOSE_DEBUG, "    VNS: New best cost: %lf\n", current_cost);
            best_cost = current_cost;
            memcpy(best_tour, current_tour, (n + 1) * sizeof(int));
        } else {
            memcpy(current_tour, best_tour, (n + 1) * sizeof(int));
            current_cost = best_cost;
        }

        cost_recorder_add(recorder, current_cost);
    }

    tsp_solution_update_if_better(solution, best_tour, best_cost);

    free(best_tour);
    free(current_tour);
}

static void free_vns_config(void *config) {
    free(config);
}

TspAlgorithm vns_create(const VNSConfig config) {
    VNSConfig *cfg_copy = malloc(sizeof(VNSConfig));
    check_alloc(cfg_copy);
    *cfg_copy = config;

    return (TspAlgorithm){
        .name = "VNS",
        .config = cfg_copy,
        .run = run_vns,
        .free_config = free_vns_config
    };
}
