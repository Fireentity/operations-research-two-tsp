#ifndef TEST_INSTANCES_H
#define TEST_INSTANCES_H

#include "tsp_instance.h"
#include "random.h"
#include <math.h>
#include <stdlib.h>

// --- Common Constants ---
#define EPSILON_EXACT     1e-4
#define EPSILON_HEURISTIC 1.0

#define TIME_LIMIT_HEURISTIC 2.0
#define TIME_LIMIT_EXACT     15.0

// --- TSPLIB Instances (Burma14) ---
static const int BURMA14_SIZE = 14;
static const double BURMA14_OPT_COST = 30.8785;

static const Node BURMA14_NODES[] = {
    {16.47, 96.10}, {16.47, 94.44}, {20.09, 92.54}, {22.39, 93.37},
    {25.23, 97.24}, {22.00, 96.05}, {20.47, 97.02}, {17.20, 96.29},
    {16.30, 97.38}, {14.05, 98.12}, {16.53, 97.38}, {21.52, 95.59},
    {19.41, 97.13}, {20.09, 94.55}
};

static inline TspInstance *create_burma14_instance(void) {
    return tsp_instance_create(BURMA14_NODES, BURMA14_SIZE);
}

// --- Random Instance ---
static inline TspInstance *create_random_instance_100(void) {
    TspGenerationArea area = {
        .x_square = 0,
        .y_square = 0,
        .square_side = 1000
    };
    return tsp_instance_create_random(100, area);
}

// --- Geometric Instances ---
static inline TspInstance *create_hexagon_instance(void) {
    // Hexagon side 1. Optimal = 6.0
    Node nodes[] = {
        {0.0, 0.0}, {1.0, 0.0}, {2.0, 0.0},
        {2.0, 1.0}, {1.0, 1.0}, {0.0, 1.0}
    };
    return tsp_instance_create(nodes, 6);
}

static inline TspInstance *create_square_instance(void) {
    // 10x10 Square. Optimal = 40.0
    Node nodes[] = {
        {0, 0}, {10, 0}, {10, 10}, {0, 10}
    };
    return tsp_instance_create(nodes, 4);
}

static inline TspInstance *create_circle_instance(int n, double radius) {
    Node *nodes = malloc(n * sizeof(Node));
    if (!nodes) return NULL;

    for(int i=0; i<n; i++) {
        double angle = 2.0 * 3.1415926535 * i / n;
        nodes[i].x = radius * cos(angle);
        nodes[i].y = radius * sin(angle);
    }

    TspInstance *inst = tsp_instance_create(nodes, n);
    free(nodes);
    return inst;
}

// --- Test Runner Prototypes ---
void run_utility_tests(void);
void run_parser_tests(void);
void run_grasp_nn_helpers_tests(void);

void run_local_search_tests(void);
void run_n_opt_tests(void);
void run_subtour_separator_tests(void);

void run_nn_tests(void);
void run_em_tests(void);
void run_vns_tests(void);
void run_ts_tests(void);
void run_grasp_tests(void);
void run_genetic_tests(void);

void run_exact_tests(void);
void run_hard_fixing_tests(void);
void run_local_branching_tests(void);

#endif // TEST_INSTANCES_H