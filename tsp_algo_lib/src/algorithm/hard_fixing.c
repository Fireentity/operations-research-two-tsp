#include "hard_fixing.h"
#include "matheuristic_utils.h"
#include "cplex_solver_wrapper.h"
#include "time_limiter.h"
#include "logger.h"
#include "c_util.h"
#include "random.h"
#include <stdlib.h>

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

    if_verbose(VERBOSE_INFO, "HardFixing: FixRate=%.2f, Time=%.2f\n",
               cfg->fixing_rate, cfg->time_limit);

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
    CplexSolverContext *ctx = cplex_solver_create(inst);
    if (!ctx) return;

    if (cplex_solver_build_base_model(ctx, inst) != 0) {
        cplex_solver_destroy(ctx);
        return;
    }

    int *tour = tsp_malloc((n + 1) * sizeof(int));

    tsp_solution_get_tour(sol, tour);

    cplex_solver_add_mip_start(ctx, n, tour);

    int fixed_count = 0;
    RandomState rng;
    random_init(&rng, cfg->seed);

    for (int i = 0; i < n; i++) {
        int u = tour[i];
        int v = tour[i + 1];
        if (random_double(&rng) < cfg->fixing_rate) {
            cplex_solver_fix_edge(ctx, u, v, 1.0, n);
            fixed_count++;
        }
    }

    cplex_solver_install_sec_callback(ctx, inst);

    double remaining = time_limiter_get_remaining(&timer);
    cplex_solver_set_time_limit(ctx, remaining);

    if (cplex_solver_optimize(ctx) == 0 && cplex_solver_has_solution(ctx)) {
        double cost = 0.0;
        cplex_solver_extract_solution(ctx, &cost);
        const double *x = cplex_solver_get_x(ctx);

        cplex_solver_reconstruct_tour(n, x, tour);
        tsp_solution_update_if_better(sol, tour, cost);
        cost_recorder_add(rec, cost);

        if_verbose(VERBOSE_INFO, "HardFixing: CPLEX improved solution to %.2f\n", cost);
    }

    tsp_free(tour);
    cplex_solver_destroy(ctx);
#endif
}

TspAlgorithm hard_fixing_create(HardFixingConfig config) {
    HardFixingConfig *c = tsp_malloc(sizeof(HardFixingConfig));

    *c = config;
    return (TspAlgorithm){
        .name = "Hard Fixing Matheuristic",
        .run = run_hard_fixing,
        .config = c,
        .free_config = free_hf_config
    };
}
