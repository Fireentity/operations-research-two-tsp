#ifndef TSP_ALGORITHM_H
#define TSP_ALGORITHM_H

union ExtendedTspAlgorithm;

typedef struct TspAlgorithm TspAlgorithm;

struct TspAlgorithm
{
    union ExtendedTspAlgorithm;

    void (*solve)(const TspAlgorithm* tsp_algorithm,
                  unsigned long* tour,
                  unsigned long number_of_nodes,
                  const double* edge_cost_array,
                  double* cost);
};


#endif //TSP_ALGORITHM_H
