#include "grasp.h"
#include "c_util.h"
#include "algorithms.h"
#include "time_limiter.h"
#include "logger.h"
#include <stdlib.h>

#include "constants.h"

// Internal runner function matching TspSolverFn signature
static void run_grasp(const TspInstance *instance,
                      TspSolution *solution,
                      const void *config_void,
                      CostRecorder *recorder) {
    const GraspConfig *cfg = config_void;
    const double p3 = 1.0 - cfg->p1 - cfg->p2;

    if_verbose(VERBOSE_DEBUG, "  GRASP (p1=%.2f, p2=%.2f, p3=%.2f, t=%.2f)\n",
               cfg->p1, cfg->p2, p3, cfg->time_limit);

    const int number_of_nodes = tsp_instance_get_num_nodes(instance);
    const double *edge_cost_array = tsp_instance_get_cost_matrix(instance);

    // Initialize timer
    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    // Allocate working buffers
    int *current_tour = malloc((number_of_nodes + 1) * sizeof(int));
    check_alloc(current_tour);

    // Get initial solution state (if any)
    tsp_solution_get_tour(solution, current_tour);

    // Prepare starting nodes for multi-start
    int *starting_nodes = malloc(number_of_nodes * sizeof(int));
    check_alloc(starting_nodes);
    for (int i = 0; i < number_of_nodes; i++) starting_nodes[i] = i;
    shuffle_int_array(starting_nodes, number_of_nodes);

    double current_cost;
    int iteration = 0;

    // Main GRASP Loop
    while (!time_limiter_is_over(&timer) && iteration < number_of_nodes) {
        const int start_node = starting_nodes[iteration];

        // 1. Constructive Phase: Randomized Greedy
        const int res = grasp_nearest_neighbor_tour(
            start_node,
            current_tour,
            number_of_nodes,
            edge_cost_array,
            &current_cost,
            cfg->p1,
            cfg->p2,
            p3
        );

        if (res == 0) {
            // 2. Local Search Phase: 2-Opt
            double improvement = two_opt(current_tour, number_of_nodes, edge_cost_array, timer);
            current_cost += improvement;

            cost_recorder_add(recorder, current_cost);

            // 3. Update Global Solution
            if (tsp_solution_update_if_better(solution, current_tour, current_cost)) {
                if_verbose(VERBOSE_DEBUG, "    GRASP [Iter %d]: New best cost: %lf\n", iteration, current_cost);
            }
        }
        iteration++;
    }

    if_verbose(VERBOSE_DEBUG, "  GRASP: Finished after %d iterations.\n", iteration);

    // Cleanup
    free(starting_nodes);
    free(current_tour);
}

static void free_grasp_config(void *config) {
    free(config);
}

TspAlgorithm grasp_create(GraspConfig config) {
    GraspConfig *cfg_copy = malloc(sizeof(GraspConfig));
    check_alloc(cfg_copy);
    *cfg_copy = config;

    return (TspAlgorithm){
        .name = "GRASP",
        .config = cfg_copy,
        .run = run_grasp,
        .free_config = free_grasp_config
    };
}
