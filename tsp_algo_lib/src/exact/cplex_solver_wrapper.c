#include "cplex_solver_wrapper.h"
#include "logger.h"
#include "c_util.h"
#include <stdlib.h>
#include <stdio.h>

int xpos(int i, int j, int num_nodes) {
    if (i == j) return -1;
    if (i > j) return xpos(j, i, num_nodes);
    return i * num_nodes + j - (i + 1) * (i + 2) / 2;
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

    // Disable screen output to avoid cluttering stdout
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
    char *colname_ptr = colname; // Helper pointer for CPLEX API

    // Add Variables (x_i_j)
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double cost = costs[i * n + j];
            double lb = 0.0, ub = 1.0;

            sprintf(colname, "x_%d_%d", i + 1, j + 1);

            status = CPXnewcols(ctx->env, ctx->lp, 1,
                                &cost, &lb, &ub,
                                &binary, &colname_ptr);
            if (status) return status;
        }
    }

    // Add Degree Constraints
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

        // Pass address of the pointer here as well
        status = CPXaddrows(ctx->env, ctx->lp,
                            0, 1, nnz,
                            &rhs, &sense,
                            &matbeg, indices, values,
                            NULL, &colname_ptr);
        if (status) break;
    }

    free(indices);
    free(values);
    return status;
}

int cplex_solver_optimize(CplexSolverContext *ctx) {
    return CPXmipopt(ctx->env, ctx->lp);
}

bool cplex_solver_has_solution(CplexSolverContext *ctx) {
    int status = CPXgetstat(ctx->env, ctx->lp);
    // Check if we found an optimal solution or a feasible one (time limit)
    if (status == CPXMIP_OPTIMAL ||
        status == CPXMIP_OPTIMAL_TOL ||
        status == CPXMIP_TIME_LIM_FEAS ||
        status == CPXMIP_MEM_LIM_FEAS) {
        return true;
    }
    return false;
}

int cplex_solver_extract_solution(CplexSolverContext *ctx, double *out_cost) {
    // Only attempt to get X if a solution exists
    int status = CPXgetx(ctx->env, ctx->lp, ctx->x_star, 0, ctx->num_cols - 1);
    if (status) return status;

    if (out_cost) {
        status = CPXgetobjval(ctx->env, ctx->lp, out_cost);
    }
    return status;
}

int cplex_solver_add_sec(CplexSolverContext *ctx,
                         const TspInstance *inst,
                         const int *component_nodes,
                         int comp_size) {
    int n = tsp_instance_get_num_nodes(inst);
    int max_edges = comp_size * (comp_size - 1) / 2;

    int *indices = malloc(max_edges * sizeof(int));
    double *values = malloc(max_edges * sizeof(double));
    check_alloc(indices);
    check_alloc(values);

    int nnz = 0;
    for (int i = 0; i < comp_size; i++) {
        for (int j = i + 1; j < comp_size; j++) {
            indices[nnz] = xpos(component_nodes[i],
                                component_nodes[j], n);
            values[nnz++] = 1.0;
        }
    }

    double rhs = comp_size - 1;
    char sense = 'L';
    int matbeg = 0;

    // Correctly handle string pointer for CPXaddrows
    char *name_literal = "SEC";
    char **name_ptr = &name_literal;

    int status = CPXaddrows(ctx->env, ctx->lp,
                            0, 1, nnz,
                            &rhs, &sense,
                            &matbeg, indices, values,
                            NULL, name_ptr);

    free(indices);
    free(values);
    return status;
}

const double *cplex_solver_get_x(const CplexSolverContext *ctx) {
    return ctx->x_star;
}

int cplex_solver_get_num_cols(const CplexSolverContext *ctx) {
    return ctx->num_cols;
}

void *cplex_solver_get_env(const CplexSolverContext *ctx) {
    return ctx->env;
}

void *cplex_solver_get_lp(const CplexSolverContext *ctx) {
    return ctx->lp;
}

#else

struct CplexSolverContext { int dummy; };

CplexSolverContext *cplex_solver_create(const TspInstance *inst) { (void)inst; return NULL; }
void cplex_solver_destroy(CplexSolverContext *ctx) { (void)ctx; }
int cplex_solver_build_base_model(CplexSolverContext *ctx, const TspInstance *inst) { (void)ctx; (void)inst; return -1; }
int cplex_solver_optimize(CplexSolverContext *ctx) { (void)ctx; return -1; }
bool cplex_solver_has_solution(CplexSolverContext *ctx) { (void)ctx; return false; }
int cplex_solver_extract_solution(CplexSolverContext *ctx, double *out_cost) { (void)ctx; (void)out_cost; return -1; }
int cplex_solver_add_sec(CplexSolverContext *ctx, const TspInstance *inst, const int *c, int s) { (void)ctx; (void)inst; (void)c; (void)s; return -1; }
const double *cplex_solver_get_x(const CplexSolverContext *ctx) { (void)ctx; return NULL; }
int cplex_solver_get_num_cols(const CplexSolverContext *ctx) { (void)ctx; return 0; }
void *cplex_solver_get_env(const CplexSolverContext *ctx) { (void)ctx; return NULL; }
void *cplex_solver_get_lp(const CplexSolverContext *ctx) { (void)ctx; return NULL; }

#endif