#ifndef HARD_FIXING_H
#define HARD_FIXING_H

#include "tsp_algorithm.h"
#include <stdint.h>

typedef enum {
    HF_HEURISTIC_NN,
    HF_HEURISTIC_EXTRA_MILEAGE,
    HF_HEURISTIC_VNS,
    HF_HEURISTIC_TABU,
    HF_HEURISTIC_GRASP
} HFHeuristicType;

typedef struct {
    double time_limit;
    double fixing_rate;
    double heuristic_time_ratio;
    HFHeuristicType heuristic_type;
    uint64_t seed;
    void *heuristic_args; // Optional heuristic-specific configuration (NULL uses defaults)
} HardFixingConfig;

TspAlgorithm hard_fixing_create(HardFixingConfig config);

#endif