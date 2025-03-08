#ifndef TSP_INSTANCE_H
#define TSP_INSTANCE_H

#include "tsp_params.h"

typedef struct
{
    long x;
    long y;
} Node;

typedef struct TspInstance TspInstance;

const TspInstance* initialize_random_tsp_instance(const TspParams* params);
const double* get_edge_cost_array(const TspInstance* instance);
long get_number_of_nodes(const TspInstance* instance);

#endif //TSP_INSTANCE_H
