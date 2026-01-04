#ifndef HARD_FIXING_H
#define HARD_FIXING_H

#include "tsp_algorithm.h"
#include <stdint.h>

#include "heuristic_types.h"

typedef struct {
    double time_limit;
    double fixing_rate;
    double heuristic_time_ratio;
    double min_time_slice;
    double time_slice_factor;
    HeuristicType heuristic_type;
    uint64_t seed;
    void *heuristic_args; // Optional heuristic-specific configuration (NULL uses defaults)
} HardFixingConfig;

TspAlgorithm hard_fixing_create(HardFixingConfig config);

#endif
