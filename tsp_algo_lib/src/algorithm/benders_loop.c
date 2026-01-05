#include "benders_loop.h"
#include "cplex_solver_wrapper.h"
#include "subtour_separator.h"
#include "logger.h"
#include "c_util.h"
#include "time_limiter.h"
#include "local_search.h"
#include "tsp_math.h"
#include <stdlib.h>

static void run_benders(const TspInstance *inst,
                        TspSolution *sol,
                        const void *cfg_void,
                        CostRecorder *rec) {
#ifdef ENABLE_CPLEX
    const BendersConfig *cfg = cfg_void;
    int n = tsp_instance_get_num_nodes(inst);
    const double *costs = tsp_instance_get_cost_matrix(inst);

    CplexSolverContext *ctx = cplex_solver_create(inst);
    if (!ctx) return;

    if (cplex_solver_build_base_model(ctx, inst)) {
        cplex_solver_destroy(ctx);
        return;
    }

    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    ConnectedComponents *cc = connected_components_create(n);
    bool optimal_found = false;

    int *tour = tsp_malloc((n + 1) * sizeof(int));

    for (int it = 0; it < cfg->max_iterations; it++) {
        double remaining = time_limiter_get_remaining(&timer);

        if (remaining <= 0.0) {
            if_verbose(VERBOSE_INFO, "Benders: time limit reached at iter %d\n", it);
            break;
        }

        cplex_solver_set_time_limit(ctx, remaining);

        if (cplex_solver_optimize(ctx) != 0) {
            if_verbose(VERBOSE_DEBUG, "Benders: optimization failed at iter %d\n", it);
            break;
        }

        if (!cplex_solver_has_solution(ctx)) {
            if_verbose(VERBOSE_INFO, "Benders: no integer solution at iter %d\n", it);
            break;
        }

        double lp_obj = 0.0;
        if (cplex_solver_extract_solution(ctx, &lp_obj) != 0) break;

        const double *x = cplex_solver_get_x(ctx);
        find_connected_components(cc, n, x);

        if (cc->num_components == 1) {
            cplex_solver_reconstruct_tour(n, x, tour);

            tsp_solution_update_if_better(sol, tour, lp_obj);
            cost_recorder_add(rec, lp_obj);

            if_verbose(VERBOSE_INFO, "Benders: optimal solution found at iter %d (Cost: %.2f)\n", it, lp_obj);
            optimal_found = true;
            break;
        }

        if_verbose(VERBOSE_DEBUG, "Benders: iter %d, %d subtours found\n", it, cc->num_components);

        for (int c = 1; c <= cc->num_components; c++) {
            int sz = 0;
            int *nodes = tsp_malloc(n * sizeof(int));

            for (int i = 0; i < n; i++)
                if (cc->component_of_node[i] == c)
                    nodes[sz++] = i;

            cplex_solver_add_sec(ctx, inst, nodes, sz);
            tsp_free(nodes);
        }
    }

    // Fallback: If optimal not found, try to patch the last integer solution
    if (!optimal_found && cplex_solver_has_solution(ctx)) {
        if_verbose(VERBOSE_INFO, "Benders: Attempting fallback patching + 2-Opt...\n");

        const double *x = cplex_solver_get_x(ctx);

        cplex_solver_reconstruct_tour(n, x, tour);

        double patched_cost = calculate_tour_cost(tour, n, costs);

        // Refinement with 2-Opt
        double remaining = time_limiter_get_remaining(&timer);
        if (remaining < 2) remaining = 2;

        TimeLimiter patch_timer = time_limiter_create(remaining);
        time_limiter_start(&patch_timer);

        double improvement = two_opt(tour, n, costs, patch_timer);
        patched_cost += improvement;

        if (tsp_solution_update_if_better(sol, tour, patched_cost)) {
            cost_recorder_add(rec, patched_cost);
            if_verbose(VERBOSE_INFO, "Benders: Fallback solution accepted (Cost: %.2f)\n", patched_cost);
        }
    }

    tsp_free(tour);
    connected_components_destroy(cc);
    cplex_solver_destroy(ctx);
#else
    (void) inst; (void) sol; (void) cfg_void; (void) rec;
    if_verbose(VERBOSE_INFO, "[ERROR] CPLEX not enabled\n");
#endif
}

static void free_benders_config(void *cfg) {
    tsp_free(cfg);
}

TspAlgorithm benders_create(BendersConfig cfg) {
    BendersConfig *c = tsp_malloc(sizeof(BendersConfig));
    *c = cfg;
    return (TspAlgorithm){
        .name = "Benders Decomposition",
        .run = run_benders,
        .config = c,
        .free_config = free_benders_config,
        .clone_config = NULL
    };
}