#ifndef TSP_SOLUTION_H
#define TSP_SOLUTION_H

#include <stdbool.h>

#include "enums.h"
#include "tsp_instance.h"

typedef struct TspSolution TspSolution;

const TspSolution* init_solution(const TspInstance* instance);
double calculate_solution_cost(const TspSolution* solution);
FeasibilityResult solve_with_nearest_neighbor(const TspSolution* solution);
FeasibilityResult check_solution_feasibility(const TspSolution* solution);
const int* get_tour(const TspSolution* instance);
void plot_solution(const TspSolution* sol);

#endif //TSP_SOLUTION_H
