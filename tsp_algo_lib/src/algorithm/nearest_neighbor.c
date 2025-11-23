#include "nearest_neighbor.h"
#include "algorithms.h"
#include "time_limiter.h"
#include "c_util.h"
#include "logger.h"
#include <stdlib.h>

#include "constants.h"

static void run_nn(const TspInstance *instance,
                   TspSolution *solution,
                   const void *config_void,
                   CostRecorder *recorder) {
    const NNConfig *cfg = config_void;
    const int n = tsp_instance_get_num_nodes(instance);
    const double *costs = tsp_instance_get_cost_matrix(instance);

    if_verbose(VERBOSE_DEBUG, "  NN: Time limit set to %.2fs.\n", cfg->time_limit);
    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    int *current_tour = malloc((n + 1) * sizeof(int));
    check_alloc(current_tour);

    // Generate Initial Solution (Single Shot - Random Start)
    double init_cost;
    // TODO remove rand here, put a specific rand function
    if (nearest_neighbor_tour(rand() % n, current_tour, n, costs, &init_cost) == 0) {
        tsp_solution_update_if_better(solution, current_tour, init_cost);
    }

    // Improvement Phase (Multi-start NN + 2-Opt)
    int *starting_nodes = malloc(n * sizeof(int));
    check_alloc(starting_nodes);
    for (int i = 0; i < n; i++) starting_nodes[i] = i;
    shuffle_int_array(starting_nodes, n);

    int iteration = 0;
    double current_cost;

    while (!time_limiter_is_over(&timer) && iteration < n) {
        const int start_node = starting_nodes[iteration];

        const int res = nearest_neighbor_tour(start_node, current_tour, n, costs, &current_cost);

        if (res == 0) {
            // Apply 2-Opt Local Search
            current_cost += two_opt(current_tour, n, costs, timer);

            cost_recorder_add(recorder, current_cost);

            if (tsp_solution_update_if_better(solution, current_tour, current_cost)) {
                if_verbose(VERBOSE_DEBUG, "    NN [Iter %d]: New best cost: %lf\n", iteration, current_cost);
            }
        }
        iteration++;
    }

    free(starting_nodes);
    free(current_tour);
}

static void free_nn_config(void *config) {
    free(config);
}

TspAlgorithm nn_create(const NNConfig config) {
    NNConfig *cfg_copy = malloc(sizeof(NNConfig));
    check_alloc(cfg_copy);
    *cfg_copy = config;

    return (TspAlgorithm){
        .name = "Nearest Neighbor",
        .config = cfg_copy,
        .run = run_nn,
        .free_config = free_nn_config
    };
}
