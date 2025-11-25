#ifndef VARIABLE_NEIGHBORHOOD_SEARCH_H
#define VARIABLE_NEIGHBORHOOD_SEARCH_H

#include "tsp_algorithm.h"

typedef struct {
    int min_k;
    int max_k;
    int kick_repetition;
    double time_limit;
    int max_stagnation;
} VNSConfig;

/**
 * @brief Creates a VNS algorithm strategy.
 */
TspAlgorithm vns_create(VNSConfig config);

#endif // VARIABLE_NEIGHBORHOOD_SEARCH_H