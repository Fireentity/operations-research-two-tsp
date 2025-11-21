#ifndef VARIABLE_NEIGHBORHOOD_SEARCH_H
#define VARIABLE_NEIGHBORHOOD_SEARCH_H
#include "tsp_algorithm.h"

/**
 * @brief Configuration for the Variable Neighborhood Search algorithm.
 */
typedef struct {
    const int kick_repetition; /**< Number of kick repetitions. */
    const double time_limit; /**< Maximum allowed time for the search. */
    const int n_opt;
} VariableNeighborhoodSearch;

/**
 * @brief Initializes a TSP algorithm using the Variable Neighborhood Search (VNS) approach.
 *
 * @param kick_repetition Number of perturbations applied in the shaking phase.
 * @param time_limit Maximum allowed execution time in seconds.
 * @return Pointer to a TspAlgorithm instance configured for VNS.
 */
const TspAlgorithm *init_vns(int kick_repetition, int n_opt, double time_limit);


#endif //VARIABLE_NEIGHBORHOOD_SEARCH_H