#include "cplex_solver_wrapper.h"
#include "subtour_separator.h"
#include "logger.h"
#include "c_util.h"
#include <stdlib.h>
#include <stdio.h>

int xpos(int i, int j, int num_nodes) {
    if (i == j) return -1;
    if (i > j) return xpos(j, i, num_nodes);
    return i * num_nodes + j - (i + 1) * (i + 2) / 2;
}

void cplex_solver_reconstruct_tour(int n, const double *x, int *tour) {
    int *vis = calloc(n, sizeof(int));
    check_alloc(vis);
    tour[0] = 0;
    vis[0] = 1;
    int cur = 0;
    for (int k = 1; k < n; k++) {
        for (int j = 0; j < n; j++) {
            if (!vis[j] && x[xpos(cur, j, n)] > 0.5) {
                tour[k] = j;
                vis[j] = 1;
                cur = j;
                break;
            }
        }
    }
    tour[n] = tour[0];
    free(vis);
}

#ifdef ENABLE_CPLEX
#include <ilcplex/cplex.h>

struct CplexSolverContext {
    CPXENVptr env;
    CPXLPptr lp;
    int num_cols;
    double *x_star;
};

CplexSolverContext *cplex_solver_create(const TspInstance *inst) {
    int status = 0;
    CplexSolverContext *ctx = malloc(sizeof *ctx);
    check_alloc(ctx);

    ctx->env = CPXopenCPLEX(&status);
    if (status) {
        free(ctx);
        return NULL;
    }

    ctx->lp = CPXcreateprob(ctx->env, &status, "TSP_Model");
    if (status) {
        CPXcloseCPLEX(&ctx->env);
        free(ctx);
        return NULL;
    }

    int n = tsp_instance_get_num_nodes(inst);
    ctx->num_cols = n * (n - 1) / 2;
    ctx->x_star = calloc(ctx->num_cols, sizeof(double));
    check_alloc(ctx->x_star);

    CPXsetintparam(ctx->env, CPX_PARAM_SCRIND, CPX_OFF);
    return ctx;
}

void cplex_solver_destroy(CplexSolverContext *ctx) {
    if (!ctx) return;
    if (ctx->lp) CPXfreeprob(ctx->env, &ctx->lp);
    if (ctx->env) CPXcloseCPLEX(&ctx->env);
    free(ctx->x_star);
    free(ctx);
}

int cplex_solver_build_base_model(CplexSolverContext *ctx, const TspInstance *inst) {
    int n = tsp_instance_get_num_nodes(inst);
    const double *costs = tsp_instance_get_cost_matrix(inst);
    int status = 0;

    char binary = 'B';
    char colname[64];
    char *colname_ptr = colname;

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double cost = costs[i * n + j];
            double lb = 0.0, ub = 1.0;
            sprintf(colname, "x_%d_%d", i + 1, j + 1);
            status = CPXnewcols(ctx->env, ctx->lp, 1, &cost, &lb, &ub, &binary, &colname_ptr);
            if (status) return status;
        }
    }

    double rhs = 2.0;
    char sense = 'E';
    int *indices = malloc(n * sizeof(int));
    double *values = malloc(n * sizeof(double));
    check_alloc(indices);
    check_alloc(values);

    for (int h = 0; h < n; h++) {
        int nnz = 0;
        for (int i = 0; i < n; i++) {
            if (i == h) continue;
            indices[nnz] = xpos(h, i, n);
            values[nnz++] = 1.0;
        }
        int matbeg = 0;
        sprintf(colname, "deg_%d", h + 1);
        status = CPXaddrows(ctx->env, ctx->lp, 0, 1, nnz, &rhs, &sense, &matbeg, indices, values, NULL, &colname_ptr);
        if (status) break;
    }

    free(indices);
    free(values);
    return status;
}

int cplex_solver_fix_edge(CplexSolverContext *ctx, int u, int v, double value, int num_nodes) {
    int index = xpos(u, v, num_nodes);
    char lu = 'B';
    double bd = value;
    return CPXchgbds(ctx->env, ctx->lp, 1, &index, &lu, &bd);
}

void cplex_solver_set_time_limit(CplexSolverContext *ctx, double seconds) {
    CPXsetdblparam(ctx->env, CPX_PARAM_TILIM, seconds);
}

typedef struct {
    const TspInstance *inst;
    int num_cols;
} CallbackCtx;

static int CPXPUBLIC lazy_sec_callback(CPXCALLBACKCONTEXTptr context, CPXLONG contextid, void *userhandle) {
    (void) contextid;
    CallbackCtx *cb_ctx = userhandle;
    int n = tsp_instance_get_num_nodes(cb_ctx->inst);

    double *x_star = malloc(cb_ctx->num_cols * sizeof(double));
    check_alloc(x_star);
    double objval;
    int status = CPXcallbackgetcandidatepoint(context, x_star, 0, cb_ctx->num_cols - 1, &objval);
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
            for (int i = 0; i < n; i++) {
                if (cc->component_of_node[i] == c) nodes[comp_size++] = i;
            }

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

            CPXcallbackrejectcandidate(context, 1, nnz, &rhs, &sense, &matbeg, ind, val);

            free(nodes);
            free(ind);
            free(val);
        }
    }
    connected_components_destroy(cc);
    free(x_star);
    return 0;
}

int cplex_solver_install_sec_callback(CplexSolverContext *ctx, const TspInstance *inst) {
    CallbackCtx *cb = malloc(sizeof(CallbackCtx));
    check_alloc(cb);
    cb->inst = inst;
    cb->num_cols = ctx->num_cols;

    return CPXcallbacksetfunc(ctx->env, ctx->lp, CPX_CALLBACKCONTEXT_CANDIDATE, lazy_sec_callback, cb);
}

int cplex_solver_optimize(CplexSolverContext *ctx) {
    return CPXmipopt(ctx->env, ctx->lp);
}

bool cplex_solver_has_solution(CplexSolverContext *ctx) {
    int status = CPXgetstat(ctx->env, ctx->lp);
    return (status == CPXMIP_OPTIMAL || status == CPXMIP_OPTIMAL_TOL ||
            status == CPXMIP_TIME_LIM_FEAS || status == CPXMIP_MEM_LIM_FEAS);
}

int cplex_solver_extract_solution(CplexSolverContext *ctx, double *out_cost) {
    int status = CPXgetx(ctx->env, ctx->lp, ctx->x_star, 0, ctx->num_cols - 1);
    if (!status && out_cost) {
        status = CPXgetobjval(ctx->env, ctx->lp, out_cost);
    }
    return status;
}

const double *cplex_solver_get_x(const CplexSolverContext *ctx) {
    return ctx->x_star;
}

int cplex_solver_get_num_cols(const CplexSolverContext *ctx) {
    return ctx->num_cols;
}

int cplex_solver_add_sec(CplexSolverContext *ctx, const TspInstance *inst, const int *nodes, int comp_size) {
    int n = tsp_instance_get_num_nodes(inst);
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
    double rhs = comp_size - 1;
    char sense = 'L';
    int matbeg = 0;
    char *name = "SEC";
    int status = CPXaddrows(ctx->env, ctx->lp, 0, 1, nnz, &rhs, &sense, &matbeg, ind, val, NULL, &name);
    free(ind);
    free(val);
    return status;
}

int cplex_solver_add_mip_start(CplexSolverContext *ctx, int num_nodes, const int *tour) {
    int nzcnt = num_nodes;

    int *indices = malloc(nzcnt * sizeof(int));
    double *values = malloc(nzcnt * sizeof(double));
    check_alloc(indices);
    check_alloc(values);

    for (int i = 0; i < num_nodes; i++) {
        int u = tour[i];
        int v = tour[i + 1];

        int idx = xpos(u, v, num_nodes);

        indices[i] = idx;
        values[i] = 1.0;
    }

    int mcnt = 1;
    int beg = 0;

    int status = CPXaddmipstarts(ctx->env, ctx->lp, mcnt, nzcnt,
                                 &beg, indices, values,
                                 NULL, NULL);

    if (status) {
        if_verbose(VERBOSE_INFO, "[CPLEX Warn] Failed to add MIP start, status: %d\n", status);
    }

    free(indices);
    free(values);
    return status;
}

void *cplex_solver_get_env(const CplexSolverContext *ctx) { return ctx->env; }
void *cplex_solver_get_lp(const CplexSolverContext *ctx) { return ctx->lp; }

#else

struct CplexSolverContext {
    int dummy;
};
CplexSolverContext *cplex_solver_create(const TspInstance *inst) { return NULL; }
void cplex_solver_destroy(CplexSolverContext *ctx) {
}
int cplex_solver_build_base_model(CplexSolverContext *ctx, const TspInstance *inst) { return -1; }
int cplex_solver_fix_edge(CplexSolverContext *ctx, int u, int v, double val, int n) { return 0; }
int cplex_solver_install_sec_callback(CplexSolverContext *ctx, const TspInstance *i) { return 0; }
void cplex_solver_set_time_limit(CplexSolverContext *ctx, double s) {
}
int cplex_solver_optimize(CplexSolverContext *ctx) { return -1; }
bool cplex_solver_has_solution(CplexSolverContext *ctx) { return false; }
int cplex_solver_extract_solution(CplexSolverContext *ctx, double *out_cost) { return -1; }
const double *cplex_solver_get_x(const CplexSolverContext *ctx) { return NULL; }
int cplex_solver_get_num_cols(const CplexSolverContext *ctx) { return 0; }
int cplex_solver_add_sec(CplexSolverContext *ctx, const TspInstance *inst, const int *c, int s) { return -1; }
int cplex_solver_add_mip_start(CplexSolverContext *c, int n, const int *t) { return 0; }
#endif
