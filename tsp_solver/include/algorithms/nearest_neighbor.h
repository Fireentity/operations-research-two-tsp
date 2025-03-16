#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include <tsp_algorithm.h>

/**
 * @brief Structure representing the nearest neighbor algorithm configuration.
 */
typedef struct
{
    const double time_limit; /**< Maximum allowed time for the algorithm execution. */
} NearestNeighbor;

/**
 * @brief Initializes the nearest neighbor TSP algorithm.
 *
 * @param time_limit The time limit for the algorithm execution.
 * @return Pointer to a TspAlgorithm instance configured to use the nearest neighbor approach.
 */
const TspAlgorithm* init_nearest_neighbor(double time_limit);

#endif //NEAREST_NEIGHBOR_H
