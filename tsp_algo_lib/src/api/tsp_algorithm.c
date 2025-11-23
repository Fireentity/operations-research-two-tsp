#include "tsp_algorithm.h"
#include "logger.h"

void tsp_algorithm_run(const TspAlgorithm *algo,
                       const TspInstance *instance,
                       TspSolution *solution,
                       CostRecorder *recorder) {
    if (!algo || !algo->run) return;

    if_verbose(VERBOSE_INFO, "Running Algorithm: %s\n", algo->name);
    algo->run(instance, solution, algo->config, recorder);
}

void tsp_algorithm_destroy(TspAlgorithm *algo) {
    if (!algo || !algo->config || !algo->free_config) return;
    algo->free_config(algo->config);
}
