#ifndef GRASP_H
#define GRASP_H

#include "tsp_algorithm.h"

/**
 * @brief Configuration structure for GRASP algorithm.
 */
typedef struct {
    double time_limit;
    double p1;
    double p2;
} GraspConfig;

/**
 * @brief Creates a GRASP algorithm strategy.
 *
 * @param config The configuration for GRASP.
 * @return A TspAlgorithm structure ready to run.
 */
TspAlgorithm grasp_create(GraspConfig config);

#endif // GRASP_H