#include "grasp.h"
#include "c_util.h"
#include "time_limiter.h"
#include "logger.h"
#include <stdlib.h>
#include "constructive.h"
#include "local_search.h"

static void run_grasp(const TspInstance *instance,
                      TspSolution *solution,
                      const void *config_void,
                      CostRecorder *recorder) {
    const GraspConfig *cfg = config_void;
    RandomState rng;
    random_init(&rng, cfg->seed);
    if_verbose(VERBOSE_INFO, "GRASP: RCL=%d, Prob=%.2f, Stagnation=%d, Time=%.2f\n",
               cfg->rcl_size, cfg->probability, cfg->max_stagnation, cfg->time_limit);

    const int n = tsp_instance_get_num_nodes(instance);
    const double *costs = tsp_instance_get_cost_matrix(instance);

    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    int *current_tour = tsp_malloc((n + 1) * sizeof(int));


    // Get initial solution state (if any)
    tsp_solution_get_tour(solution, current_tour);

    // Prepare starting nodes for multi-start
    int *starting_nodes = tsp_malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) starting_nodes[i] = i;
    shuffle_int_array(starting_nodes, n, &rng);

    double current_cost;
    int iter = 0;
    int stagnation_counter = 0;

    // Main GRASP Loop
    // Continue until: Time is over OR all start nodes visited OR stagnation reached
    while (!time_limiter_is_over(&timer) && iter < n && stagnation_counter < cfg->max_stagnation) {
        const int start_node = starting_nodes[iter];

        const int res = grasp_nearest_neighbor_tour(
            start_node,
            current_tour,
            n,
            costs,
            &current_cost,
            cfg->rcl_size,
            cfg->probability,
            &rng
        );

        if (res == 0) {
            // Local Search Phase: 2-Opt
            current_cost += two_opt(current_tour, n, costs, timer);

            cost_recorder_add(recorder, current_cost);

            // 3. Update Global Solution
            if (tsp_solution_update_if_better(solution, current_tour, current_cost)) {
                if_verbose(VERBOSE_DEBUG, "\tGRASP [Iter %d, Start %d]: New best cost: %.2f\n", iter, start_node,
                           current_cost);
                stagnation_counter = 0; // Reset stagnation on improvement
            } else {
                stagnation_counter++; // Increment stagnation
            }
        }
        iter++;
    }

    if (time_limiter_is_over(&timer)) {
        if_verbose(VERBOSE_INFO, "\tTabu: time is over\n");
    }
    if (stagnation_counter >= cfg->max_stagnation) {
        if_verbose(VERBOSE_INFO, "\tGRASP: Max stagnation reached.\n");
    }
    if_verbose(VERBOSE_DEBUG, "GRASP: Finished after %d iterations.\n", iter);

    // Cleanup
    tsp_free(starting_nodes);
    tsp_free(current_tour);
}

static void *grasp_clone_config(const void *config, uint64_t seed_offset) {
    const GraspConfig *src = config;
    GraspConfig *dest = tsp_malloc(sizeof(GraspConfig));
    *dest = *src;
    dest->seed += seed_offset;
    return dest;
}

static void free_grasp_config(void *config) {
    tsp_free(config);
}


TspAlgorithm grasp_create(GraspConfig config) {
    GraspConfig *cfg_copy = tsp_malloc(sizeof(GraspConfig));

    *cfg_copy = config;

    return (TspAlgorithm){
        .name = "GRASP",
        .config = cfg_copy,
        .run = run_grasp,
        .free_config = free_grasp_config,
        .clone_config = grasp_clone_config
    };
}
