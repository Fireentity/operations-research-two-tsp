#include <math.h>
#include "tsp_solution.h"
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include "math_util.h"
#include "constants.h"
#include "enums.h"
#include "c_util.h"

struct TspSolution
{
    double cost;
    int* const tour;
    const TspInstance* const instance;
};

const int* get_tour(const TspSolution* instance)
{
    return instance->tour;
}

double calculate_solution_cost(const TspSolution* solution)
{
    return calculate_tour_cost(
        solution->tour,
        get_number_of_nodes(solution->instance),
        get_edge_cost_array(solution->instance)
    );
}

const TspSolution* init_solution(const TspInstance* instance)
{
    const long number_of_nodes = get_number_of_nodes(instance);

    int* tour = calloc(number_of_nodes + 1, sizeof(int));
    check_alloc(tour);
    for (int i = 0; i < number_of_nodes; i++)
        tour[i] = i;
    tour[number_of_nodes] = tour[0];

    TspSolution* solution_ptr = malloc(sizeof(TspSolution));
    check_alloc(solution_ptr);
    const TspSolution solution = {
        .cost = calculate_tour_cost(tour, number_of_nodes, get_edge_cost_array(instance)),
        .tour = tour,
        .instance = instance
    };
    memcpy(solution_ptr, &solution, sizeof(solution));

    return solution_ptr;
}


FeasibilityResult check_solution_feasibility(const TspSolution* solution)
{
    const long number_of_nodes = get_number_of_nodes(solution->instance);
    int counter[number_of_nodes];
    memset(counter, 0, number_of_nodes * sizeof(int));

    for (int i = 0; i < number_of_nodes; i++)
    {
        // Checks that each value in the solution array is a valid index in the TSP problem's node array
        if (solution->tour[i] < 0 || solution->tour[i] > number_of_nodes - 1)
        {
            return UNINITIALIZED_ENTRY;
        }

        counter[solution->tour[i]]++;

        // Ensures each index appears exactly once; if an index appears more than once or not at all, the TSP solution is infeasible
        // This is because in a valid TSP tour, each node must be visited exactly once to form a Hamiltonian cycle
        if (counter[solution->tour[i]] != 1)
        {
            return DUPLICATED_ENTRY;
        }
    }

    // Recalculate the total cost of the current solution
    const double calculated_cost = calculate_solution_cost(solution);

    // Check if the stored cost is approximately equal to the recalculated cost within an epsilon margin
    // This accounts for floating-point precision errors
    if (fabs(solution->cost - calculated_cost) > EPSILON)
    {
        return NON_MATCHING_COST;
    }

    return FEASIBLE;
}

void plot_solution(const TspSolution* sol)
{
    plot_tour(sol->tour, sol->instance);
}
