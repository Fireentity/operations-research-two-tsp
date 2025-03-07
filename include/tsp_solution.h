#ifndef TSP_SOLUTION_H
#define TSP_SOLUTION_H

#include <stdbool.h>
#include "tsp_instance.h"

typedef enum
{
    DUPLICATED_ENTRY,
    UNINITIALIZED_ENTRY,
    NON_MATCHING_COST,
    FEASIBLE
} FeasibilityResult;

typedef struct TspSolution* TspSolution;

//TspSolutionState* initialize_nearest_neighbor_solution(TspInstance* instance);
bool check_solution_feasibility(const TspInstance* instance);
const int* get_solution(const struct TspSolution* instance);

#endif //TSP_SOLUTION_H
