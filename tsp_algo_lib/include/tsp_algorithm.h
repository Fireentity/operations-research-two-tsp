#ifndef TSP_ALGORITHM_H
#define TSP_ALGORITHM_H


typedef struct TspAlgorithm TspAlgorithm;

#define DEFINE_TSP_ALGORITHM() struct TspAlgorithm \
{ \
    ExtendedTspAlgorithm; \
\
    void (*solve)(const TspAlgorithm* tsp_algorithm, \
                  unsigned long* tour, \
                  unsigned long number_of_nodes, \
                  const double* edge_cost_array, \
                  double* cost); \
};


#endif //TSP_ALGORITHM_H
