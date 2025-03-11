#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

typedef struct TspAlgorithm TspAlgorithm; // forward declaration

typedef struct
{
    const double time_limit;
} NearestNeighbor;

const TspAlgorithm* init_nearest_neighbor(double time_limit);

#endif //NEAREST_NEIGHBOR_H
