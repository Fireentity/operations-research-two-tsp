#include "branch_and_cut.h"
#include "cplex_solver_wrapper.h"
#include "subtour_separator.h"
#include "logger.h"
#include "c_util.h"
#include <stdlib.h>

#ifdef ENABLE_CPLEX
#include <ilcplex/cplex.h>

typedef struct {
    const TspInstance *inst;
    int num_cols;
} CallbackCtx;

static int CPXPUBLIC lazy_constraint_callback(
    CPXCALLBACKCONTEXTptr context,
    CPXLONG contextid,
    void *userhandle) {
    CallbackCtx *ctx = userhandle;
    int n = tsp_instance_get_num_nodes(ctx->inst);

    double *x_star = malloc(ctx->num_cols * sizeof(double));
    check_alloc(x_star);

    double objval;
    int status = CPXcallbackgetcandidatepoint(
        context, x_star, 0, ctx->num_cols - 1, &objval
    );
    if (status) {
        free(x_star);
        return status;
    }

    ConnectedComponents *cc = connected_components_create(n);
    find_connected_components(cc, n, x_star);

    if (cc->num_components > 1) {
        for (int c = 1; c <= cc->num_components; c++) {
            int comp_size = 0;
            int *nodes = malloc(n * sizeof(int));
            check_alloc(nodes);

            for (int i = 0; i < n; i++)
                if (cc->component_of_node[i] == c)
                    nodes[comp_size++] = i;

            int max_edges = comp_size * (comp_size - 1) / 2;
            int *ind = malloc(max_edges * sizeof(int));
            double *val = malloc(max_edges * sizeof(double));
            check_alloc(ind);
            check_alloc(val);

            int nnz = 0;
            for (int i = 0; i < comp_size; i++) {
                for (int j = i + 1; j < comp_size; j++) {
                    ind[nnz] = xpos(nodes[i], nodes[j], n);
                    val[nnz] = 1.0;
                    nnz++;
                }
            }

            double rhs = (double) (comp_size - 1);
            char sense = 'L';
            int matbeg = 0;

            CPXcallbackrejectcandidate(
                context, 1, nnz, &rhs, &sense, &matbeg, ind, val
            );

            free(nodes);
            free(ind);
            free(val);
        }
    }

    connected_components_destroy(cc);
    free(x_star);
    return 0;
}

static void run_bc(const TspInstance *inst,
                   TspSolution *sol,
                   const void *cfg_void,
                   CostRecorder *rec) {
    const BranchCutConfig *cfg = cfg_void;
    (void) rec;

    CplexSolverContext *ctx = cplex_solver_create(inst);
    if (!ctx) return;

    cplex_solver_build_base_model(ctx, inst);

    CPXENVptr env = cplex_solver_get_env(ctx);
    CPXLPptr lp = cplex_solver_get_lp(ctx);
    int num_cols = cplex_solver_get_num_cols(ctx);

    CPXsetdblparam(env, CPX_PARAM_TILIM, cfg->time_limit);
    if (cfg->num_threads > 0)
        CPXsetintparam(env, CPX_PARAM_THREADS, cfg->num_threads);

    CallbackCtx cb_ctx = {.inst = inst, .num_cols = num_cols};
    CPXcallbacksetfunc(
        env, lp,
        CPX_CALLBACKCONTEXT_CANDIDATE,
        lazy_constraint_callback,
        &cb_ctx
    );

    int status = CPXmipopt(env, lp);
    if (!status) {
        double *x = malloc(num_cols * sizeof(double));
        check_alloc(x);

        CPXgetx(env, lp, x, 0, num_cols - 1);

        int n = tsp_instance_get_num_nodes(inst);
        int *tour = malloc((n + 1) * sizeof(int));
        check_alloc(tour);

        int *vis = calloc(n, sizeof(int));
        check_alloc(vis);

        tour[0] = 0;
        vis[0] = 1;
        int cur = 0;

        for (int i = 1; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (!vis[j] && x[xpos(cur, j, n)] > 0.5) {
                    tour[i] = j;
                    vis[j] = 1;
                    cur = j;
                    break;
                }
            }
        }
        tour[n] = tour[0];

        double cost;
        CPXgetobjval(env, lp, &cost);
        tsp_solution_update_if_better(sol, tour, cost);

        free(vis);
        free(tour);
        free(x);
    }

    cplex_solver_destroy(ctx);
}

#else

static void run_bc(const TspInstance *i,
                   TspSolution *s,
                   const void *c,
                   CostRecorder *r) {
    (void) i;
    (void) s;
    (void) c;
    (void) r;
    if_verbose(VERBOSE_INFO, "[ERROR] Branch & Cut unavailable (no CPLEX)\n");
}

#endif

static void free_bc_config(void *cfg) { free(cfg); }

TspAlgorithm branch_and_cut_create(BranchCutConfig cfg) {
    BranchCutConfig *c = malloc(sizeof(BranchCutConfig));
    check_alloc(c);
    *c = cfg;
    return (TspAlgorithm){
        .name = "Branch and Cut",
        .run = run_bc,
        .config = c,
        .free_config = free_bc_config
    };
}
