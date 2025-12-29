#include "local_branching.h"
#include "matheuristic_utils.h"
#include "cplex_solver_wrapper.h"
#include "time_limiter.h"
#include "logger.h"
#include "c_util.h"
#include <stdlib.h>

static void free_lb_config(void *cfg_void) {
    LocalBranchingConfig *cfg = cfg_void;
    matheuristic_free_args(cfg->heuristic_args);
    tsp_free(cfg);
}

static void run_local_branching(const TspInstance *inst,
                                TspSolution *sol,
                                const void *cfg_void,
                                CostRecorder *rec) {
    const LocalBranchingConfig *cfg = cfg_void;
    int n = tsp_instance_get_num_nodes(inst);

    if_verbose(VERBOSE_INFO, "LocalBranching: k=%d, Time=%.2f\n", cfg->k, cfg->time_limit);

    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

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

    int iter = 0;
    bool improved = true;

    while (improved && !time_limiter_is_over(&timer)) {
        improved = false;
        iter++;

        CplexSolverContext *ctx = cplex_solver_create(inst);
        if (!ctx) break;

        if (cplex_solver_build_base_model(ctx, inst) != 0) {
            cplex_solver_destroy(ctx);
            break;
        }

        cplex_solver_add_mip_start(ctx, n, current_tour);
        cplex_solver_add_local_branching_constraint(ctx, n, current_tour, cfg->k);
        cplex_solver_install_sec_callback(ctx, inst);

        double remaining = time_limiter_get_remaining(&timer);
        cplex_solver_set_time_limit(ctx, remaining);

        if (cplex_solver_optimize(ctx) == 0 && cplex_solver_has_solution(ctx)) {
            double cost = 0.0;
            cplex_solver_extract_solution(ctx, &cost);

            if (cost < current_cost - 1e-6) {
                const double *x = cplex_solver_get_x(ctx);
                cplex_solver_reconstruct_tour(n, x, current_tour);
                tsp_solution_update_if_better(sol, current_tour, cost);
                cost_recorder_add(rec, cost);
                current_cost = cost;
                improved = true;
                if_verbose(VERBOSE_INFO, "LB [Iter %d]: Improved to %.2f\n", iter, cost);
            }
        }
        cplex_solver_destroy(ctx);
    }
    tsp_free(current_tour);
#endif
}

TspAlgorithm local_branching_create(LocalBranchingConfig config) {
    LocalBranchingConfig *c = tsp_malloc(sizeof(LocalBranchingConfig));

    *c = config;
    return (TspAlgorithm){
        .name = "Local Branching Matheuristic",
        .run = run_local_branching,
        .config = c,
        .free_config = free_lb_config,
        .clone_config = NULL // TODO
    };
}
