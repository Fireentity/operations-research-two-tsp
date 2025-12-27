#ifndef BENDERS_LOOP_H
#define BENDERS_LOOP_H

#include "tsp_algorithm.h"

typedef struct {
    double time_limit;
    int max_iterations;
} BendersConfig;

TspAlgorithm benders_create(BendersConfig config);

#endif // BENDERS_LOOP_H
