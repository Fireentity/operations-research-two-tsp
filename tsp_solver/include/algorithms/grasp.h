#ifndef GRASP_H
#define GRASP_H
#include <tsp_algorithm.h>

typedef struct
{
    const double time_limit;
} Grasp;

/**
 * Initialize a grasp algorithm for the Traveling Salesman Problem (TSP).
 *
 * @param time_limit The maximum time allowed for the search.
 * @return A pointer to an initialized TspAlgorithm structure.
 */
const TspAlgorithm* init_grasp(double time_limit);
#endif //GRASP_H
