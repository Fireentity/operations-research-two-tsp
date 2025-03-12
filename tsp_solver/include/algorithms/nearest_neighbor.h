#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include <extended_algorithms.h>
#include <tsp_algorithm.h>

struct NearestNeighbor
{
    const double time_limit;
};

const TspAlgorithm* init_nearest_neighbor(double time_limit);

#endif //NEAREST_NEIGHBOR_H
