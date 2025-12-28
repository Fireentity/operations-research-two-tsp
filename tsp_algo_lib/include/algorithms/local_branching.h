#ifndef LOCAL_BRANCHING_H
#define LOCAL_BRANCHING_H

#include "tsp_algorithm.h"
#include <stdint.h>
#include "heuristic_types.h"

typedef struct {
    double time_limit;
    int k;
    double heuristic_time_ratio;
    HeuristicType heuristic_type;
    uint64_t seed;
    void *heuristic_args; // Optional heuristic-specific configuration (NULL uses defaults)
} LocalBranchingConfig;

TspAlgorithm local_branching_create(LocalBranchingConfig config);

#endif
