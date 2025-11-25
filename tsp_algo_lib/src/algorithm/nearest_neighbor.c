#include "nearest_neighbor.h"
#include "algorithms.h"
#include "time_limiter.h"
#include "c_util.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "constants.h"
#include "tsp_math_util.h"

/* Building a clean NN tour from a fixed start improves multi-start quality */
static double build_nn_tour(int start_node, int *tour, int *visited, int n, const double *costs) {
    memset(visited, 0, n * sizeof(int));

    tour[0] = start_node;
    visited[start_node] = 1;

    int current = start_node;
    double cost = 0.0;

    for (int step = 1; step < n; step++) {
        int next = -1;
        double best = DBL_MAX;

        const double *row = &costs[current * n];

        for (int j = 0; j < n; j++) {
            if (!visited[j] && row[j] < best) {
                best = row[j];
                next = j;
            }
        }

        if (next == -1) break;

        tour[step] = next;
        visited[next] = 1;
        cost += best;
        current = next;
    }

    tour[n] = start_node;
    cost += costs[current * n + start_node];
    return cost;
}

static void run_nn(const TspInstance *instance,
                   TspSolution *solution,
                   const void *config_void,
                   CostRecorder *recorder) {
    const NNConfig *cfg = config_void;
    const int n = tsp_instance_get_num_nodes(instance);
    const double *costs = tsp_instance_get_cost_matrix(instance);

    if_verbose(VERBOSE_INFO, "NN: time limit = %.2f\n", cfg->time_limit);

    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    int *tour = malloc((n + 1) * sizeof(int));
    check_alloc(tour);

    int *visited = malloc(n * sizeof(int));
    check_alloc(visited);

    int *starts = malloc(n * sizeof(int));
    check_alloc(starts);

    for (int i = 0; i < n; i++) starts[i] = i;
    shuffle_int_array(starts, n);

    int iter = 0;

    while (!time_limiter_is_over(&timer) && iter < n) {
        const int s = starts[iter];

        double cost = build_nn_tour(s, tour, visited, n, costs);
        cost += two_opt(tour, n, costs, timer);

        cost_recorder_add(recorder, cost);

        if (tsp_solution_update_if_better(solution, tour, cost)) {
            if_verbose(VERBOSE_DEBUG,
                       "\tNN: new best %.2f from start %d\n",
                       cost, s);
        }

        iter++;
    }

    if (time_limiter_is_over(&timer))
        if_verbose(VERBOSE_INFO, "\tNN: time is over\n");

    free(starts);
    free(visited);
    free(tour);
}

static void free_nn_config(void *config) {
    free(config);
}

TspAlgorithm nn_create(const NNConfig config) {
    NNConfig *cpy = malloc(sizeof(NNConfig));
    check_alloc(cpy);
    *cpy = config;

    return (TspAlgorithm){
        .name = "Nearest Neighbor",
        .config = cpy,
        .run = run_nn,
        .free_config = free_nn_config
    };
}
