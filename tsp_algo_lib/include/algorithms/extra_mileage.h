#ifndef EXTRA_MILEAGE_H
#define EXTRA_MILEAGE_H

#include <stdint.h>

#include "tsp_algorithm.h"

typedef struct {
    double time_limit;
    uint64_t seed;
} EMConfig;

/**
 * @brief Creates an Extra Mileage algorithm strategy.
 */
TspAlgorithm em_create(EMConfig config);

#endif // EXTRA_MILEAGE_H