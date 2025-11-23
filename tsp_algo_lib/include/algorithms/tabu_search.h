#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H

#include "tsp_algorithm.h"

typedef struct {
    int tenure;
    int max_stagnation;
    double time_limit;
} TabuConfig;

/**
 * @brief Creates a Tabu Search algorithm strategy.
 */
TspAlgorithm tabu_create(TabuConfig config);

#endif // TABU_SEARCH_H