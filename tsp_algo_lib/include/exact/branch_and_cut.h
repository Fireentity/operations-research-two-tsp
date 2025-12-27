#ifndef BRANCH_AND_CUT_H
#define BRANCH_AND_CUT_H

#include "tsp_algorithm.h"

typedef struct {
    double time_limit;
    int num_threads;
} BranchCutConfig;

TspAlgorithm branch_and_cut_create(BranchCutConfig config);

#endif // BRANCH_AND_CUT_H
