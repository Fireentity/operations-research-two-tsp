#ifndef TSP_ALGORITHM_H
#define TSP_ALGORITHM_H

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
struct TspAlgorithm
{
    TspExtendedAlgorithms* const extended_algorithms; /**< Pointer to extended algorithms. */
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
     */
    void (*solve)(const TspAlgorithm* tsp_algorithm,
                  int* tour,
                  int number_of_nodes,
                  const double* edge_cost_array,
                  double* cost);
};

#endif //TSP_ALGORITHM_H
