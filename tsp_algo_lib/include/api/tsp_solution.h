#ifndef TSP_SOLUTION_H
#define TSP_SOLUTION_H

#include <stdbool.h>
#include "feasibility_result.h"
#include "tsp_error.h"

// Forward declarations
typedef struct TspInstance TspInstance;
typedef struct TspSolution TspSolution;

TspSolution *tsp_solution_create(const TspInstance *instance);

TspSolution *tsp_solution_create_with_tour(const TspInstance *instance, const int *tour);

void tsp_solution_destroy(TspSolution *self);

FeasibilityResult tsp_solution_check_feasibility(TspSolution *self);

double tsp_solution_get_cost(TspSolution *self);

void tsp_solution_get_tour(TspSolution *self, int *tour_buffer);

bool tsp_solution_update_if_better(TspSolution *self, const int *new_tour, double new_cost);

TspError tsp_solution_save(TspSolution *self, const char *path);

TspError tsp_solution_load(TspSolution *self, const char *path);

#endif // TSP_SOLUTION_H