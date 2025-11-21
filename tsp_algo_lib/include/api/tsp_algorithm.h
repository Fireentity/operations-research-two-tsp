#ifndef TSP_ALGORITHM_H
#define TSP_ALGORITHM_H
#include <pthread.h>

#include "costs_plotter.h"
#include "tsp_solution.h"

/**
 * @brief TSP algorithm interface.
 */
typedef struct TspAlgorithm TspAlgorithm;

/**
 * @brief Union for extended TSP algorithm implementations.
 */
typedef union TspExtendedAlgorithms TspExtendedAlgorithms;

/**
 * @brief Structure representing a TSP algorithm.
 */
struct TspAlgorithm {
    TspExtendedAlgorithms *const extended; /**< Pointer to extended algorithms. */
    void (*const solve)(const TspAlgorithm *tsp_algorithm,
                        const TspInstance *instance,
                        const TspSolution *solution,
                        const CostsPlotter *plotter);

    void (*const free)(const TspAlgorithm *self);
};

#endif //TSP_ALGORITHM_H