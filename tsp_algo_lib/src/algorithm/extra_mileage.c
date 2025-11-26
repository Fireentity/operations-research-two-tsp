#include "extra_mileage.h"
#include "constructive.h"
#include "local_search.h"
#include "c_util.h"
#include "logger.h"
#include <stdlib.h>

static void run_em(const TspInstance *instance,
                   TspSolution *solution,
                   const void *config_void,
                   CostRecorder *recorder) {
    const EMConfig *cfg = config_void;
    const int n = tsp_instance_get_num_nodes(instance);
    const double *costs = tsp_instance_get_cost_matrix(instance);

    if_verbose(VERBOSE_INFO, "Extra Mileage: Time=%.2f\n", cfg->time_limit);

    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    int *tour = malloc((n + 1) * sizeof(int));
    check_alloc(tour);

    double cost = 0.0;

    // 1. Construct
    if (extra_mileage_tour(tour, n, costs, &cost) == 0) {
        cost_recorder_add(recorder, cost);
        
        // 2. Improve (2-Opt)
        // Note: Extra Mileage is deterministic, so we run it once.
        // 2-Opt is added for consistency with other algos in the lib.
        double improvement = two_opt(tour, n, costs, timer);
        cost += improvement;
        
        cost_recorder_add(recorder, cost);
        
        if (tsp_solution_update_if_better(solution, tour, cost)) {
            if_verbose(VERBOSE_DEBUG, "  EM: Solution found cost=%.2f\n", cost);
        }
    }

    free(tour);
}

static void free_em_config(void *config) {
    free(config);
}

TspAlgorithm em_create(EMConfig config) {
    EMConfig *cfg_copy = malloc(sizeof(EMConfig));
    check_alloc(cfg_copy);
    *cfg_copy = config;

    return (TspAlgorithm){
        .name = "Extra Mileage",
        .config = cfg_copy,
        .run = run_em,
        .free_config = free_em_config
    };
}