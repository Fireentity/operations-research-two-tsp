#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include <tsp_algorithm.h>

typedef struct
{
    const double time_limit;
} NearestNeighbor;

const TspAlgorithm* init_nearest_neighbor(double time_limit);

#endif //NEAREST_NEIGHBOR_H
