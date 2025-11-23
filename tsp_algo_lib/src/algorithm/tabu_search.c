#include "tabu_search.h"
#include "algorithms.h"
#include "tsp_math_util.h"
#include "time_limiter.h"
#include "c_util.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "constants.h"

static void run_tabu(const TspInstance *instance,
                     TspSolution *solution,
                     const void *config_void,
                     CostRecorder *recorder) {
    const TabuConfig *cfg = config_void;
    const int n = tsp_instance_get_num_nodes(instance);
    const double *costs = tsp_instance_get_cost_matrix(instance);

    if_verbose(VERBOSE_DEBUG, "  Tabu Search (Tenure=%d, Stagnation=%d, t=%.2f)\n",
               cfg->tenure, cfg->max_stagnation, cfg->time_limit);

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

    int *tabu_matrix = calloc(n * n, sizeof(int));
    check_alloc(tabu_matrix);

    int no_improv = 0;
    int iteration = 0;

    while (!time_limiter_is_over(&timer) && no_improv < cfg->max_stagnation) {
        iteration++;
        int best_i = -1, best_j = -1;
        double best_delta = DBL_MAX;

        for (int i = 1; i < n - 1; i++) {
            for (int j = i + 1; j < n; j++) {
                const int a = current_tour[i - 1];
                const int b = current_tour[i];
                const int c = current_tour[j];
                const int d = current_tour[j + 1];

                const double delta = costs[a * n + c] + costs[b * n + d] -
                                     (costs[a * n + b] + costs[c * n + d]);

                const int tabu_idx = i * n + j;
                const int is_tabu = (tabu_matrix[tabu_idx] > iteration);

                if (is_tabu && (current_cost + delta >= best_cost - EPSILON)) {
                    continue;
                }

                if (delta < best_delta) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        if (best_i != -1 && best_j != -1) {
            const int edges[2] = {best_i - 1, best_j};
            compute_n_opt_move(2, current_tour, edges, n);
            current_cost += best_delta;

            tabu_matrix[best_i * n + best_j] = iteration + cfg->tenure;

            if (current_cost < best_cost - EPSILON) {
                best_cost = current_cost;
                memcpy(best_tour, current_tour, (n + 1) * sizeof(int));
                no_improv = 0;
                if_verbose(VERBOSE_DEBUG, "    Tabu: New best: %lf\n", best_cost);
            } else {
                no_improv++;
            }
        } else {
            if_verbose(VERBOSE_DEBUG, "    Tabu: No valid moves found.\n");
            break;
        }

        cost_recorder_add(recorder, current_cost);
    }

    tsp_solution_update_if_better(solution, best_tour, best_cost);

    free(tabu_matrix);
    free(best_tour);
    free(current_tour);
}

static void free_tabu_config(void *config) {
    free(config);
}

TspAlgorithm tabu_create(const TabuConfig config) {
    TabuConfig *cfg_copy = malloc(sizeof(TabuConfig));
    check_alloc(cfg_copy);
    *cfg_copy = config;

    return (TspAlgorithm){
        .name = "Tabu Search",
        .config = cfg_copy,
        .run = run_tabu,
        .free_config = free_tabu_config
    };
}
