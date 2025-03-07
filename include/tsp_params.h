//
// Created by croce on 07/03/2025.
//

#ifndef TSP_PARAMS_H
#define TSP_PARAMS_H
#include <stdbool.h>

typedef struct {
    long x_square;
    long y_square;
    long square_side;
} Rectangle;

typedef struct {
    long number_of_nodes;
    long seed;
    Rectangle generation_area;
    bool help;
} TspParams;

#endif //TSP_PARAMS_H
