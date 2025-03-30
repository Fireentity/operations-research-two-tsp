#ifndef TSP_ALGORITHM_H
#define TSP_ALGORITHM_H
#include <pthread.h>

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
    /**
     * @brief Solves the TSP.
     *
     * Computes a tour and its cost.
     *
     * @param tsp_algorithm Pointer to the TSP algorithm instance.
     * @param tour Array where the tour is stored.
     * @param number_of_nodes Number of nodes.
     * @param edge_cost_array Array of edge costs.
     * @param cost Pointer to store the total cost.
     * @param mutex Mutex for thread safe access to solution
     */
    void (*const solve)(const TspAlgorithm *tsp_algorithm,
                        int *tour,
                        int number_of_nodes,
                        const double *edge_cost_array,
                        double *cost,
                        pthread_mutex_t *mutex);
    /**
     * @brief Improve the TSP.
     *
     * Computes a tour and its cost.
     *
     * @param tsp_algorithm Pointer to the TSP algorithm instance.
     * @param tour Array where the tour is stored.
     * @param number_of_nodes Number of nodes.
     * @param edge_cost_array Array of edge costs.
     * @param cost Pointer to store the total cost.
     * @param mutex Mutex for thread safe access to solution
     */
    void (*const improve)(const TspAlgorithm *tsp_algorithm,
                        int *tour,
                        int number_of_nodes,
                        const double *edge_cost_array,
                        double *cost,
                        pthread_mutex_t *mutex);

    /**
     * @brief Frees the memory allocated for the TSP algorithm instance.
     *
     * @param self Pointer to the TSP algorithm instance to be freed.
     */
    void (*const free)(const TspAlgorithm *self);
};

#endif //TSP_ALGORITHM_H
