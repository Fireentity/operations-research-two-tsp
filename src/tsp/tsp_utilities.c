#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tsp_utilities.h"
#include "parsing_util.h"

#define EPSILON 1e-6

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

TspInstance* initialize_tsp_instance(const Rectangle rect, const long number_of_nodes, const long seed = 0)
{
    srand(seed);
    Node* nodes = malloc(number_of_nodes * sizeof(Node));
    if (!nodes)
    {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < number_of_nodes; i++)
    {
        nodes[i].x = rect.x_square + rand() % (rect.square_side + 1);
        nodes[i].y = rect.y_square + rand() % (rect.square_side + 1);
    }
    TspInstance* instance = malloc(sizeof(TspInstance));
    instance->number_of_nodes = number_of_nodes;
    instance->nodes = nodes;
    instance->solution = calloc(number_of_nodes,sizeof(int));
    return nodes;
}

Node* generate_random_nodes(const Rectangle rect, const long number_of_nodes, const long seed)
{
    srand(seed);
    Node* nodes = malloc(number_of_nodes * sizeof(Node));
    if (!nodes)
    {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < number_of_nodes; i++)
    {
        nodes[i].x = rect.x_square + rand() % (rect.square_side + 1);
        nodes[i].y = rect.y_square + rand() % (rect.square_side + 1);
    }
    return nodes;
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
