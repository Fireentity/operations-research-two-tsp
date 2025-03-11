#ifndef TSP_SOLUTION_H
#define TSP_SOLUTION_H

#include <feasibility_result.h>
#include <tsp_algorithm.h>
#include <tsp_instance.h>

typedef struct TspSolution TspSolution;

TspSolution* init_solution(const TspInstance* instance);
FeasibilityResult solve(const TspAlgorithm* tsp_algorithm, TspSolution *solution);
FeasibilityResult check_solution_feasibility(const TspSolution* solution);
void plot_solution(const TspSolution *sol, const char *output_name);
void free_tsp_solution(TspSolution* solution);

#endif //TSP_SOLUTION_H
