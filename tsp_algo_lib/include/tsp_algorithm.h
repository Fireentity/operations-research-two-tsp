#ifndef TSP_ALGORITHM_H
#define TSP_ALGORITHM_H


typedef struct TspAlgorithm TspAlgorithm;

typedef union TspExtendedAlgorithms TspExtendedAlgorithms;

struct TspAlgorithm
{
    TspExtendedAlgorithms* const extended_algorithms;
    void (*solve)(const TspAlgorithm* tsp_algorithm,
                  int* tour,
                  int number_of_nodes,
                  const double* edge_cost_array,
                  double* cost);
};


#endif //TSP_ALGORITHM_H
