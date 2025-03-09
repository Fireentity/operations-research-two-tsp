#include <float.h>
#include <stdlib.h>

#include "enums.h"
#include "math_util.h"
#include "tsp_solution.h"

FeasibilityResult solve_with_nearest_neighbor(const TspSolution* solution)
{
    const TspInstance* instance = solution->instance;
    const long number_of_nodes = get_number_of_nodes(instance);
    const double* const edge_cost_array = get_edge_cost_array(instance);
    const long start = rand() % number_of_nodes;

    int* tour = solution->tour;
    long visited = 1;
    int current = tour[0];
    SWAP(tour[0], tour[start]);
    tour[number_of_nodes] = tour[0];

    while (visited < number_of_nodes)
    {
        double best_cost = DBL_MAX;
        long best_index = visited;

        for (long i = visited; i < number_of_nodes; i++)
        {
            const double cost = edge_cost_array[current * number_of_nodes + tour[i]];
            if (cost < best_cost)
            {
                best_cost = cost;
                best_index = i;
            }
        }

        SWAP(tour[visited], tour[best_index]);
        current = tour[visited];
        visited++;
    }

    *(double*)&solution->cost = calculate_solution_cost(solution);

    const FeasibilityResult result = check_solution_feasibility(solution);
    if (result != FEASIBLE)
    {
        return result;
    }
    return FEASIBLE;
}
