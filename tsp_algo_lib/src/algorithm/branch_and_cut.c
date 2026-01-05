#include "branch_and_cut.h"
#include "cplex_solver_wrapper.h"
#include "logger.h"
#include "c_util.h"
#include "time_limiter.h"
#include "constructive.h"
#include "local_search.h"
#include <stdlib.h>
#include <string.h>

#include "tsp_math.h"

static void run_bc(const TspInstance *inst,
                   TspSolution *sol,
                   const void *cfg_void,
                   CostRecorder *rec) {
#ifdef ENABLE_CPLEX
    const BranchCutConfig *cfg = cfg_void;
    int n = tsp_instance_get_num_nodes(inst);
    const double *original_costs = tsp_instance_get_cost_matrix(inst);

    CplexSolverContext *ctx = cplex_solver_create(inst);
    if (!ctx) return;

    if (cplex_solver_build_base_model(ctx, inst) != 0) {
        cplex_solver_destroy(ctx);
        return;
    }

    cplex_solver_install_sec_callback(ctx, inst);

    // Warm start from current solution if available
    if (tsp_solution_get_cost(sol) > 0) {
        int *current_tour = tsp_malloc((n + 1) * sizeof(int));
        tsp_solution_get_tour(sol, current_tour);
        cplex_solver_add_mip_start(ctx, n, current_tour);
        tsp_free(current_tour);
    }

    cplex_solver_set_time_limit(ctx, cfg->time_limit);

    TimeLimiter timer = time_limiter_create(cfg->time_limit);
    time_limiter_start(&timer);

    int status = cplex_solver_optimize(ctx);

    // Case 1: CPLEX found an integer solution
    if (cplex_solver_has_solution(ctx)) {
        double cost = 0.0;
        cplex_solver_extract_solution(ctx, &cost);
        const double *x = cplex_solver_get_x(ctx);

        int *tour = tsp_malloc((n + 1) * sizeof(int));
        cplex_solver_reconstruct_tour(n, x, tour);

        tsp_solution_update_if_better(sol, tour, cost);
        cost_recorder_add(rec, cost);
        tsp_free(tour);
    }
    // Case 2: Fallback (Time limit reached, no integer solution found)
    else if (status != 0) {
        if_verbose(VERBOSE_INFO, "BC: No integer solution found. Attempting LP-guided fallback.\n");

        double *biased_costs = tsp_malloc(n * n * sizeof(double));
        const double *x_frac = cplex_solver_get_x(ctx); // LP relaxation values

        // Create biased costs: Cost_new = Cost_old * (1 - 0.9 * x_frac)
        // If x_frac is close to 1, cost becomes very small, guiding GRASP to pick it.
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                int idx = xpos(i, j, n);
                double val = x_frac[idx]; // Fractional value [0, 1]
                double weight = 1.0 - (0.9 * val);

                biased_costs[i * n + j] = original_costs[i * n + j] * weight;
                biased_costs[j * n + i] = biased_costs[i * n + j];
            }
        }

        int *tour = tsp_malloc((n + 1) * sizeof(int));
        double dummy_cost = 0.0;
        RandomState rng;
        random_init(&rng, n);
        int res = grasp_nearest_neighbor_tour(0, tour, n, biased_costs, &dummy_cost, 5, 0.9, &rng);

        if (res == 0) {
            TimeLimiter remaining_timer = time_limiter_create(2.0); // Quick refinement
            time_limiter_start(&remaining_timer);

            two_opt(tour, n, original_costs, remaining_timer);

            double final_cost = calculate_tour_cost(tour, n, original_costs);

            if (tsp_solution_update_if_better(sol, tour, final_cost)) {
                cost_recorder_add(rec, final_cost);
                if_verbose(VERBOSE_INFO, "BC: Fallback solution found (Cost: %.2f)\n", final_cost);
            }
        }

        tsp_free(tour);
        tsp_free(biased_costs);
    }

    cplex_solver_destroy(ctx);
#else
    (void) inst; (void) sol; (void) cfg_void; (void) rec;
    if_verbose(VERBOSE_INFO, "[ERROR] Branch & Cut unavailable (no CPLEX)\n");
#endif
}

static void free_bc_config(void *cfg) { tsp_free(cfg); }

TspAlgorithm branch_and_cut_create(BranchCutConfig cfg) {
    BranchCutConfig *c = tsp_malloc(sizeof(BranchCutConfig));
    *c = cfg;
    return (TspAlgorithm){
        .name = "Branch and Cut",
        .run = run_bc,
        .config = c,
        .free_config = free_bc_config,
        .clone_config = NULL
    };
}