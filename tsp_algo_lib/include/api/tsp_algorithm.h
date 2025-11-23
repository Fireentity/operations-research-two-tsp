#ifndef TSP_ALGORITHM_H
#define TSP_ALGORITHM_H

#include "cost_recorder.h"
#include "tsp_instance.h"
#include "tsp_solution.h"


/**
 * @brief Generic function pointer for running a TSP algorithm.
 * @param instance The problem instance (read-only).
 * @param solution The solution object to update (in-place).
 * @param config Algorithm-specific configuration struct.
 * @param recorder Optional plotter for visualization.
 */
typedef void (*TspSolverFn)(const TspInstance *instance,
                            TspSolution *solution,
                            const void *config,
                            CostRecorder *recorder);

typedef struct {
    const char *name;
    void *config;
    TspSolverFn run;

    void (*free_config)(void *);
} TspAlgorithm;

/**
 * @brief Executes the algorithm on the provided solution.
 */
void tsp_algorithm_run(const TspAlgorithm *algo,
                       const TspInstance *instance,
                       TspSolution *solution,
                       CostRecorder *recorder);

void tsp_algorithm_destroy(TspAlgorithm *algo);

#endif // TSP_ALGORITHM_H
