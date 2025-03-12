#ifndef TSP_SOLUTION_H
#define TSP_SOLUTION_H

#include <feasibility_result.h>
#include <tsp_algorithm.h>
#include <tsp_instance.h>

typedef struct TspSolutionState TspSolutionState;

typedef struct TspSolution TspSolution;

struct TspSolution
{
    TspSolutionState* state;
    FeasibilityResult (*const solve)(const TspSolution* self, const TspAlgorithm* tsp_algorithm);
    FeasibilityResult (*const is_feasible)(const TspSolution* self);
    void (*const free)(const TspInstance* self);
    const int* (* const get_tour)(const TspSolution* solution);
};

TspSolution* init_solution(const TspInstance* instance);

#endif //TSP_SOLUTION_H
