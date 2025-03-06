#ifndef TSP_UTILITIES_H
#define TSP_UTILITIES_H

#define VERBOSITY 0
#include "parsing_util.h"

typedef struct
{
    long x;
    long y;
} Node;

/**
 * @struct TspInstance
 * @brief Represents an instance of the Traveling Salesman Problem (TSP).
 *
 * This structure holds all the necessary data for solving a TSP instance.
 *
 * @var TspInstance::cost
 * The total cost of the current tour.
 *
 * @var TspInstance::number_of_nodes
 * The total number of nodes in the problem.
 *
 * @var TspInstance::nodes
 * Pointer to an array of nodes, each containing coordinates.
 *
 * @var TspInstance::solution
 * Pointer to an array of indices representing the tour.
 * The first node is duplicated at the end of the array to simplify
 * the evaluation of the last edge connecting the last node back to the start.
 */
typedef struct
{
    const double** edge_cost_matrix;
    const long number_of_nodes;
    const Node *const nodes;
} TspInstance;

typedef struct
{
    const double cost;
    int *const solution;
} TspSolution;

typedef struct
{
    int *const counter;
} TspFeasibility;

Node* generate_random_nodes(Rectangle rect, long number_of_nodes, long seed);
bool check_solution_feasibility(const TspInstance* instance);

#endif
