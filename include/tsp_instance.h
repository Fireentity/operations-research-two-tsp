#ifndef TSP_INSTANCE_H
#define TSP_INSTANCE_H

#include <glob.h>
#include "cmd_options.h"

typedef struct
{
    long x;
    long y;
} Node;

typedef struct TspInstance TspInstance;

const TspInstance* init_random_tsp_instance(unsigned long number_of_nodes,
                                            int seed,
                                            Rectangle generation_area);
const double* get_edge_cost_array(const TspInstance* instance);
unsigned long get_number_of_nodes(const TspInstance* instance);
void plot_tour(const unsigned long *tour, const TspInstance* instance, const char *output_name);
const Node* get_nodes(const TspInstance* instance);
void solve_instance(TspSolver solver, unsigned long *tour, double *cost, const TspInstance *instance);

#endif //TSP_INSTANCE_H
