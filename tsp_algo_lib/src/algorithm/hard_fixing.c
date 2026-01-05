#include "hard_fixing.h"
#include "matheuristic_utils.h"
#include "cplex_solver_wrapper.h"
#include "time_limiter.h"
#include "logger.h"
#include "c_util.h"
#include "random.h"
#include <stdlib.h>

#include "constants.h"

static void free_hf_config(void *cfg_void) {
    HardFixingConfig *cfg = cfg_void;
    matheuristic_free_args(cfg->heuristic_args);
    tsp_free(cfg);
}

static void run_hard_fixing(const TspInstance *inst,
                            TspSolution *sol,
                            const void *cfg_void,
                            CostRecorder *rec) {
    const HardFixingConfig *cfg = cfg_void;
    int n = tsp_instance_get_num_nodes(inst);
    if_verbose(VERBOSE_INFO, "HardFixing: FixRate=%.2f, Time=%.2f, SliceFactor=%.2f\n",
               cfg->fixing_rate, cfg->time_limit, cfg->time_slice_factor);

    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    // Warm Start: allocate a portion of the time budget for the initial heuristic
    double heuristic_time = cfg->time_limit * cfg->heuristic_time_ratio;

    if (heuristic_time < 2.0) heuristic_time = 2.0;
    if (heuristic_time > cfg->time_limit) heuristic_time = cfg->time_limit;

    WarmStartParams ws_params = {
        .time_limit = heuristic_time,
        .heuristic_type = cfg->heuristic_type,
        .seed = cfg->seed,
        .heuristic_args = cfg->heuristic_args
    };
    matheuristic_run_warm_start(&ws_params, inst, sol, rec);

    if (time_limiter_is_over(&timer)) return;

#ifdef ENABLE_CPLEX
    int *current_tour = tsp_malloc((n + 1) * sizeof(int));

    tsp_solution_get_tour(sol, current_tour);
    double current_cost = tsp_solution_get_cost(sol);

    RandomState rng;
    random_init(&rng, cfg->seed);

    int iter = 0;

    while (!time_limiter_is_over(&timer)) {
        iter++;

        double current_remaining = time_limiter_get_remaining(&timer);

        // Determine time budget for this specific iteration based on config
        double iter_limit = current_remaining * cfg->time_slice_factor;

        // Ensure CPLEX has at least the minimum configured time to perform work
        if (iter_limit < cfg->min_time_slice) {
            iter_limit = cfg->min_time_slice;
        }

        // Hard cap: cannot exceed what is actually left
        if (iter_limit > current_remaining) {
            iter_limit = current_remaining;
        }

        if (iter_limit < 1.0) break; // Stop if time is too short to initialize CPLEX

        CplexSolverContext *ctx = cplex_solver_create(inst);
        if (!ctx) break;

        if (cplex_solver_build_base_model(ctx, inst) != 0) {
            cplex_solver_destroy(ctx);
            break;
        }

        // Use the current best solution as MIP Start to prune the search tree
        cplex_solver_add_mip_start(ctx, n, current_tour);

        // HARD FIXING: Randomly fix edges present in the current tour
        int fixed_count = 0;
        for (int i = 0; i < n; i++) {
            int u = current_tour[i];
            int v = current_tour[i + 1];

            if (random_double(&rng) < cfg->fixing_rate) {
                cplex_solver_fix_edge(ctx, u, v, 1.0, n);
                fixed_count++;
            }
        }

        cplex_solver_install_sec_callback(ctx, inst);
        cplex_solver_set_time_limit(ctx, iter_limit);

        // Track CPLEX execution time
        double start_opt = time_limiter_get_remaining(&timer);
        int status = cplex_solver_optimize(ctx);
        double end_opt = time_limiter_get_remaining(&timer);

        if_verbose(VERBOSE_DEBUG, "HF [Iter %d]: CPLEX finished in %.3fs (Limit: %.2fs)\n",
                   iter, (start_opt - end_opt), iter_limit);

        if (status == 0 && cplex_solver_has_solution(ctx)) {
            double cost = 0.0;
            cplex_solver_extract_solution(ctx, &cost);

            if (cost < current_cost - EPSILON) {
                const double *x = cplex_solver_get_x(ctx);
                cplex_solver_reconstruct_tour(n, x, current_tour);

                tsp_solution_update_if_better(sol, current_tour, cost);
                cost_recorder_add(rec, cost);

                if_verbose(VERBOSE_INFO, "HF [Iter %d]: Improved to %.2f (Fixed %d edges)\n",
                           iter, cost, fixed_count);

                // Update reference cost to center the neighborhood on the new solution
                current_cost = cost;
            }
        }

        // Destroy context to unfix variables and reset for next iteration
        cplex_solver_destroy(ctx);
    }

    tsp_free(current_tour);
#endif
}

TspAlgorithm hard_fixing_create(HardFixingConfig config) {
    HardFixingConfig *c = tsp_malloc(sizeof(HardFixingConfig));
    *c = config;
    return (TspAlgorithm){
        .name = "Hard Fixing Matheuristic",
        .run = run_hard_fixing,
        .config = c,
        .free_config = free_hf_config,
        .clone_config = NULL
    };
}