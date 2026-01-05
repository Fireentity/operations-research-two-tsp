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

/**
 * Robust tour reconstruction:
 * If the graph defined by x contains subtours (disconnected components),
 * this function will extract the component containing 'current', and then
 * simply jump to the next unvisited node to continue building a valid (but likely non-optimal) tour.
 * This effectively performs a "Naive Merge" of components.
 */
void cplex_solver_reconstruct_tour(int n, const double *x, int *tour) {
    int *vis = tsp_calloc(n, sizeof(int));
    int count = 0;
    int current = 0;

    // Start with node 0
    tour[count++] = current;
    vis[current] = 1;

    while (count < n) {
        int next = -1;

        // 1. Try to find a valid edge from 'current' in the CPLEX solution
        for (int j = 0; j < n; j++) {
            if (!vis[j] && x[xpos(current, j, n)] > 0.5) {
                next = j;
                break;
            }
        }

        // 2. If no edge found (subtour closed prematurely), perform NAIVE MERGE.
        //    Jump to the first available unvisited node.
        if (next == -1) {
            for (int j = 0; j < n; j++) {
                if (!vis[j]) {
                    next = j;
                    break;
                }
            }
        }

        // 3. Update state
        if (next != -1) {
            tour[count++] = next;
            vis[next] = 1;
            current = next;
        } else {
            // Should theoretically not happen if count < n and logic is correct
            break;
        }
    }

    tour[n] = tour[0]; // Close the loop
    tsp_free(vis);
}

#ifdef ENABLE_CPLEX
#include <ilcplex/cplex.h>

struct CplexSolverContext {
    CPXENVptr env;
    CPXLPptr lp;
    int num_cols;
    double *x_star;
    void *callback_data;
};

CplexSolverContext *cplex_solver_create(const TspInstance *inst) {
    int status = 0;
    CplexSolverContext *ctx = tsp_malloc(sizeof *ctx);


    ctx->env = CPXopenCPLEX(&status);
    if (status) {
        tsp_free(ctx);
        return NULL;
    }

    ctx->lp = CPXcreateprob(ctx->env, &status, "TSP_Model");
    if (status) {
        CPXcloseCPLEX(&ctx->env);
        tsp_free(ctx);
        return NULL;
    }

    int n = tsp_instance_get_num_nodes(inst);
    ctx->num_cols = n * (n - 1) / 2;
    ctx->x_star = tsp_calloc(ctx->num_cols, sizeof(double));

    ctx->callback_data = NULL;
    CPXsetintparam(ctx->env, CPX_PARAM_SCRIND, CPX_OFF);
    return ctx;
}

void cplex_solver_destroy(CplexSolverContext *ctx) {
    if (!ctx) return;
    if (ctx->lp) CPXfreeprob(ctx->env, &ctx->lp);
    if (ctx->env) CPXcloseCPLEX(&ctx->env);
    if (ctx->callback_data) {
        tsp_free(ctx->callback_data);
        ctx->callback_data = NULL;
    }
    tsp_free(ctx->x_star);
    tsp_free(ctx);
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
    int *indices = tsp_malloc(n * sizeof(int));
    double *values = tsp_malloc(n * sizeof(double));


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

    tsp_free(indices);
    tsp_free(values);
    return status;
}

int cplex_solver_add_local_branching_constraint(CplexSolverContext *ctx, int num_nodes, const int *tour, int k) {
    int nzcnt = num_nodes;
    int *indices = tsp_malloc(nzcnt * sizeof(int));
    double *values = tsp_malloc(nzcnt * sizeof(double));


    for (int i = 0; i < num_nodes; i++) {
        int u = tour[i];
        int v = tour[i + 1];
        indices[i] = xpos(u, v, num_nodes);
        values[i] = 1.0;
    }

    double rhs = num_nodes - k;
    char sense = 'G';
    int matbeg = 0;
    char *name = "LOCAL_BRANCHING";

    int status = CPXaddrows(ctx->env, ctx->lp, 0, 1, nzcnt, &rhs, &sense, &matbeg, indices, values, NULL, &name);

    tsp_free(indices);
    tsp_free(values);
    return status;
}

int cplex_solver_fix_edge(CplexSolverContext *ctx, int u, int v, double value, int num_nodes) {
    int index = xpos(u, v, num_nodes);
    if (index < 0 || index >= ctx->num_cols) {
        return -1;
    }
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

    double *x_star = tsp_malloc(cb_ctx->num_cols * sizeof(double));

    double objval;
    int status = CPXcallbackgetcandidatepoint(context, x_star, 0, cb_ctx->num_cols - 1, &objval);
    if (status) {
        tsp_free(x_star);
        return status;
    }

    ConnectedComponents *cc = connected_components_create(n);
    find_connected_components(cc, n, x_star);

    if (cc->num_components > 1) {
        for (int c = 1; c <= cc->num_components; c++) {
            int comp_size = 0;
            int *nodes = tsp_malloc(n * sizeof(int));

            for (int i = 0; i < n; i++) {
                if (cc->component_of_node[i] == c) nodes[comp_size++] = i;
            }

            int max_edges = comp_size * (comp_size - 1) / 2;
            int *ind = tsp_malloc(max_edges * sizeof(int));
            double *val = tsp_malloc(max_edges * sizeof(double));


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

            tsp_free(nodes);
            tsp_free(ind);
            tsp_free(val);
        }
    }
    connected_components_destroy(cc);
    tsp_free(x_star);
    return 0;
}

int cplex_solver_install_sec_callback(CplexSolverContext *ctx, const TspInstance *inst) {
    CallbackCtx *cb = tsp_malloc(sizeof(CallbackCtx));

    cb->inst = inst;
    cb->num_cols = ctx->num_cols;
    ctx->callback_data = cb;
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
    int *ind = tsp_malloc(max_edges * sizeof(int));
    double *val = tsp_malloc(max_edges * sizeof(double));


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
    tsp_free(ind);
    tsp_free(val);
    return status;
}

int cplex_solver_add_mip_start(CplexSolverContext *ctx, int num_nodes, const int *tour) {
    int nzcnt = num_nodes;

    int *indices = tsp_malloc(nzcnt * sizeof(int));
    double *values = tsp_malloc(nzcnt * sizeof(double));


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

    tsp_free(indices);
    tsp_free(values);
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