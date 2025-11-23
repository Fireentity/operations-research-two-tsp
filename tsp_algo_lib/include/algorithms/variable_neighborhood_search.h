#ifndef VARIABLE_NEIGHBORHOOD_SEARCH_H
#define VARIABLE_NEIGHBORHOOD_SEARCH_H

#include "tsp_algorithm.h"

typedef struct {
    int kick_repetition;
    int n_opt;
    double time_limit;
} VNSConfig;

/**
 * @brief Creates a VNS algorithm strategy.
 */
TspAlgorithm vns_create(VNSConfig config);

#endif // VARIABLE_NEIGHBORHOOD_SEARCH_H
