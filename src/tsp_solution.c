#include <math.h>
#include "tsp_solution.h"

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"

struct TspSolution
{
    const double cost;
    int* const solution;
    const TspInstance* instance;
};

const int* get_solution(const struct TspSolution* instance)
{
    return instance->solution;
}

double calculate_solution_cost(const struct TspSolution* solution)
{
    const int number_of_nodes = get_number_of_nodes(solution->instance);
    const double* const edge_cost_array = get_edge_cost_array(solution->instance);
    double cost = 0;
    for (int i = 0; i < number_of_nodes; i++)
    {
        cost += edge_cost_array[solution->solution[i] * number_of_nodes + solution->solution[i + 1]];
    }
    return cost;
}

FeasibilityResult check_solution_feasibility(const struct TspSolution* solution)
{
    const int number_of_nodes = get_number_of_nodes(solution->instance);
    int counter[number_of_nodes];
    for (int i = 0; i < number_of_nodes; i++)
    {
        // Checks that each value in the solution array is a valid index in the TSP problem's node array
        if (solution->solution[i] < 0 || solution->solution[i] > number_of_nodes - 1)
        {
            return UNINITIALIZED_ENTRY;
        }

        counter[solution->solution[i]]++;

        // Ensures each index appears exactly once; if an index appears more than once or not at all, the TSP solution is infeasible
        // This is because in a valid TSP tour, each node must be visited exactly once to form a Hamiltonian cycle
        if (counter[solution->solution[i]] != 1)
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

FeasibilityResult generate_nearest_neighbor_solution(struct TspSolution* solution)
{
    const FeasibilityResult result = check_solution_feasibility(solution);
    if (result != FEASIBLE)
    {
        return result;
    }



    return FEASIBLE;
}

/*switch (result)
{
case DUPLICATED_ENTRY:
    fprintf(stderr, "Nearest Neighbor method generated an unfeasible solution. Reason : DUPLICATED_ENTRY\n");
case UNINITIALIZED_ENTRY:
    fprintf(stderr, "Nearest Neighbor method generated an unfeasible solution. Reason : UNINITIALIZED_ENTRY\n");
case NON_MATCHING_COST:
    fprintf(stderr, "Nearest Neighbor method generated an unfeasible solution. Reason : NON_MATCHING_COST\n");
default:
    fprintf(stderr, "Nearest Neighbor method generated an unfeasible solution. Reason : UNKNOWN\n");;
}
fprintf(stderr, "Allocation error\n");
exit(EXIT_FAILURE);*/
