#ifndef MATHEURISTIC_UTILS_H
#define MATHEURISTIC_UTILS_H

#include "heuristic_types.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "cost_recorder.h"
#include <stdint.h>

typedef struct {
    double time_limit;
    HeuristicType heuristic_type;
    uint64_t seed;
    void *heuristic_args;
} WarmStartParams;

void matheuristic_run_warm_start(const WarmStartParams *params,
                                 const TspInstance *inst,
                                 TspSolution *sol,
                                 CostRecorder *rec);

void matheuristic_free_args(void *heuristic_args);

#endif
