#ifndef TSP_SOLUTION_H
#define TSP_SOLUTION_H

#include <stdbool.h>
#include <glob.h>

#include "enums.h"
#include "tsp_instance.h"

typedef struct TspSolution TspSolution;


TspSolution* init_solution(const TspInstance* instance);
double calculate_solution_cost(const TspSolution* solution);
FeasibilityResult solve_tsp(TspSolver solver, TspSolution *solution);
FeasibilityResult check_solution_feasibility(const TspSolution* solution);
void plot_solution(const TspSolution *sol, const char *output_name);
unsigned long tour_array_size(unsigned long number_of_nodes);


#endif //TSP_SOLUTION_H
