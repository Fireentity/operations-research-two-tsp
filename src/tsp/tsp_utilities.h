#ifndef TSP_UTILITIES_H
#define TSP_UTILITIES_H

#define VERBOSITY 0
#include "parsing_util.h"

typedef struct
{
    long x;
    long y;
} Node;

typedef struct
{
    long number_of_nodes;
    Node* solution;
} TspInstance;

Node* generate_random_nodes(Rectangle rect, long number_of_nodes, long seed);

#endif
