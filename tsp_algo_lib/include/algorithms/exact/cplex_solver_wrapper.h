#ifndef CPLEX_SOLVER_WRAPPER_H
#define CPLEX_SOLVER_WRAPPER_H

#include "tsp_instance.h"
#include <stdbool.h>

typedef struct CplexSolverContext CplexSolverContext;

int xpos(int i, int j, int num_nodes);

CplexSolverContext *cplex_solver_create(const TspInstance *inst);

void cplex_solver_destroy(CplexSolverContext *ctx);

int cplex_solver_build_base_model(CplexSolverContext *ctx, const TspInstance *inst);

int cplex_solver_fix_edge(CplexSolverContext *ctx, int u, int v, double value, int num_nodes);

int cplex_solver_install_sec_callback(CplexSolverContext *ctx, const TspInstance *inst);

void cplex_solver_set_time_limit(CplexSolverContext *ctx, double seconds);

int cplex_solver_optimize(CplexSolverContext *ctx);

bool cplex_solver_has_solution(CplexSolverContext *ctx);

int cplex_solver_extract_solution(CplexSolverContext *ctx, double *out_cost);

const double *cplex_solver_get_x(const CplexSolverContext *ctx);

int cplex_solver_get_num_cols(const CplexSolverContext *ctx);

void cplex_solver_reconstruct_tour(int n, const double *x, int *tour);

int cplex_solver_add_sec(CplexSolverContext *ctx, const TspInstance *inst, const int *component_nodes, int comp_size);

int cplex_solver_add_mip_start(CplexSolverContext *ctx, int num_nodes, const int *tour);
#ifdef ENABLE_CPLEX
void *cplex_solver_get_env(const CplexSolverContext *ctx);

void *cplex_solver_get_lp(const CplexSolverContext *ctx);
#endif

#endif
