#ifndef GENETIC_H
#define GENETIC_H

#include "tsp_algorithm.h"

typedef struct {
    double time_limit;
    int population_size;
    int elite_count;
    double mutation_rate;
    int crossover_cut_min_ratio; // e.g., 25 for 25%
    int crossover_cut_max_ratio; // e.g., 75 for 75%
} GeneticConfig;

TspAlgorithm genetic_create(GeneticConfig config);

#endif // GENETIC_H