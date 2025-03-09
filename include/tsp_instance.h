#ifndef TSP_INSTANCE_H
#define TSP_INSTANCE_H

#include "cmd_options.h"

typedef struct
{
    long x;
    long y;
} Node;

typedef struct TspInstance TspInstance;

const TspInstance* init_random_tsp_instance(unsigned int number_of_nodes,
                                            int seed,
                                            Rectangle generation_area);
const double* get_edge_cost_array(const TspInstance* instance);
long get_number_of_nodes(const TspInstance* instance);
void plot_tour(const int* tour, const TspInstance* instance);

#endif //TSP_INSTANCE_H
