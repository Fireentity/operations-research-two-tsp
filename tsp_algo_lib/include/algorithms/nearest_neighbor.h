#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include "tsp_algorithm.h"

typedef struct {
    double time_limit;
} NNConfig;

/**
 * @brief Creates a Nearest Neighbor algorithm strategy.
 */
TspAlgorithm nn_create(NNConfig config);

#endif // NEAREST_NEIGHBOR_H
