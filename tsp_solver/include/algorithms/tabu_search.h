#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H
#include <tsp_algorithm.h>

typedef struct
{
    const int tenure;
    const double time_limit;
    const int *tabu_list;
} TabuSearch;

const TspAlgorithm* init_tabu(int tenure, double time_limit);
#endif //TABU_SEARCH_H
