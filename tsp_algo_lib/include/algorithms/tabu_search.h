#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H
#include <tsp_algorithm.h>
typedef struct
{
    const int tenure;
    const int max_stagnation;
    const double time_limit;
    struct hashmap* tabu_map;
} TabuSearch;

/**
 * Initialize a tabu search algorithm for the Traveling Salesman Problem (TSP).
 *
 * @param tenure The size of the tabu list.
 * @param max_stagnation Max stagnation for the search
 * @param time_limit The maximum time allowed for the search.
 * @return A pointer to an initialized TspAlgorithm structure.
 */
const TspAlgorithm* init_tabu(int tenure, int max_stagnation, double time_limit);
#endif //TABU_SEARCH_H
