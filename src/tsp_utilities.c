#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tsp_utilities.h"
#include "parsing_util.h"
#include "constants.h"

typedef struct {
    long number_of_nodes;
    long seed;
    Rectangle generation_area;
    bool help;
} TspParams;

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

double euclidean_distance(const Node a, const Node b)
{
    const long dx = a.x - b.x;
    const long dy = a.y - b.y;
    return sqrt((double)(dx * dx + dy * dy));
}

double calculate_solution_cost(const TspInstance* instance)
{
    double cost = 0.0;
    for (int i = 0; i < instance->number_of_nodes; i++)
    {
        cost += euclidean_distance(
            instance->nodes[instance->solution[i]],
            instance->nodes[instance->solution[i + 1]]
        );
    }
    return cost;
}

bool check_solution_feasibility(const TspInstance* instance)
{
    int counter[instance->number_of_nodes];
    for (int i = 0; i < instance->number_of_nodes; i++)
    {
        // Checks that each value in the solution array is a valid index in the TSP problem's node array
        if (instance->solution[i] < 0 || instance->solution[i] > instance->number_of_nodes - 1)
        {
            return false;
        }

        counter[instance->solution[i]]++;

        // Ensures each index appears exactly once; if an index appears more than once or not at all, the TSP solution is infeasible
        // This is because in a valid TSP tour, each node must be visited exactly once to form a Hamiltonian cycle
        if (counter[instance->solution[i]] != 1)
        {
            return false;
        }
    }

    // Recalculate the total cost of the current solution
    const double calculated_cost = calculate_solution_cost(instance);

    // Check if the stored cost is approximately equal to the recalculated cost within an epsilon margin
    // This accounts for floating-point precision errors
    if (fabs(instance->cost - calculated_cost) > EPSILON)
    {
        return false;
    }

    return true;
}
