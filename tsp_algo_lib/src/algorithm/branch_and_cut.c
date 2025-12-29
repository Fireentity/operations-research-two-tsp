#include "../../include/algorithms/branch_and_cut.h"
#include "cplex_solver_wrapper.h"
#include "logger.h"
#include "c_util.h"
#include <stdlib.h>

#ifdef ENABLE_CPLEX
#include <ilcplex/cplex.h>

static void run_bc(const TspInstance *inst,
                   TspSolution *sol,
                   const void *cfg_void,
                   CostRecorder *rec) {
    const BranchCutConfig *cfg = cfg_void;
    (void) rec;

    CplexSolverContext *ctx = cplex_solver_create(inst);
    if (!ctx) return;

    if (cplex_solver_build_base_model(ctx, inst) != 0) {
        cplex_solver_destroy(ctx);
        return;
    }

    cplex_solver_install_sec_callback(ctx, inst);

    cplex_solver_set_time_limit(ctx, cfg->time_limit);

    CPXENVptr env = cplex_solver_get_env(ctx);
    if (cfg->num_threads > 0) {
        CPXsetintparam(env, CPX_PARAM_THREADS, cfg->num_threads);
    }

    if (cplex_solver_optimize(ctx) == 0 && cplex_solver_has_solution(ctx)) {
        double cost = 0.0;
        cplex_solver_extract_solution(ctx, &cost);

        int n = tsp_instance_get_num_nodes(inst);
        int *tour = tsp_malloc((n + 1) * sizeof(int));


        const double *x = cplex_solver_get_x(ctx);
        cplex_solver_reconstruct_tour(n, x, tour);

        tsp_solution_update_if_better(sol, tour, cost);

        tsp_free(tour);
    }

    cplex_solver_destroy(ctx);
}

#else

static void run_bc(const TspInstance *i, TspSolution *s, const void *c, CostRecorder *r) {
    (void) i;
    (void) s;
    (void) c;
    (void) r;
    if_verbose(VERBOSE_INFO, "[ERROR] Branch & Cut unavailable (no CPLEX)\n");
}

#endif

static void free_bc_config(void *cfg) { tsp_free(cfg); }

TspAlgorithm branch_and_cut_create(BranchCutConfig cfg) {
    BranchCutConfig *c = tsp_malloc(sizeof(BranchCutConfig));

    *c = cfg;
    return (TspAlgorithm){
        .name = "Branch and Cut",
        .run = run_bc,
        .config = c,
        .free_config = free_bc_config
    };
}
