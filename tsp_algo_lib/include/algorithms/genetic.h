#ifndef GENETIC_H
#define GENETIC_H

#include <stdint.h>

#include "tsp_algorithm.h"

typedef struct {
    double time_limit;
    int population_size;
    int elite_count;
    double mutation_rate;
    int crossover_cut_min_ratio; // e.g., 25 for 25%
    int crossover_cut_max_ratio; // e.g., 75 for 75%
    int tournament_size;
    int init_grasp_rcl_size;
    double init_grasp_prob;
    int init_grasp_percent;
    uint64_t seed;
} GeneticConfig;

TspAlgorithm genetic_create(GeneticConfig config);

#endif // GENETIC_H
