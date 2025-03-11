#include <nearest_neighbor.h>
#include <tsp_algorithm.h>

struct NearestNeighbor
{
    const double time_limit;
};

union ExtendedTspAlgorithm
{
    NearestNeighbor nearest_neighbor;
};