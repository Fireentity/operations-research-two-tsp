#ifndef GRASP_H
#define GRASP_H
#include "tsp_algorithm.h"

typedef struct
{
    const double time_limit;
    const double p1;
    const double p2;
    const double p3;
} Grasp;

/**
 * Initialize a grasp algorithm for the Traveling Salesman Problem (TSP).
 *
 * @param time_limit The maximum time allowed for the search.
 * @param p1
 * @param p2
 * @return A pointer to an initialized TspAlgorithm structure.
 */
const TspAlgorithm* init_grasp(double time_limit, double p1, double p2);
#endif //GRASP_H
