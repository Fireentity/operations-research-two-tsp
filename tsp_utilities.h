#ifndef TSP_UTILITIES_H
#define TSP_UTILITIES_H

#define VERBOSITY 0
#include "parsing/parsing_util.h"

typedef struct {
    long x;
    long y;
} Node;

Node *generate_random_points(Rectangle rect, long number_of_nodes, long seed);

#endif
