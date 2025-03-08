#include <math.h>
#include "tsp_solution.h"
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include "math_util.h"
#include "constants.h"
#include "enums.h"

struct TspSolution
{
    const double cost;
    int* const tour;
    const TspInstance* instance;
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

//TODO controllare se questa funzione deve ritornare un puntatore costante
TspSolution* initialize_solution(const TspInstance* instance)
{
    TspSolution* solution = malloc(sizeof(TspSolution));
    int* tour = calloc(get_number_of_nodes(instance), sizeof(int));
    for (int i = 1; i < get_number_of_nodes(instance); i++) tour[i] = i;
    const double cost = calculate_tour_cost(tour, get_number_of_nodes(instance), get_edge_cost_array(instance));
    const TspSolution stack_solution = {.cost = cost, .tour = tour, .instance = instance};
    memcpy(solution, &stack_solution, sizeof(TspSolution));
    return solution;
}


FeasibilityResult check_solution_feasibility(const TspSolution* solution)
{
    const long number_of_nodes = get_number_of_nodes(solution->instance);
    int counter[number_of_nodes];
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

FeasibilityResult solve_with_nearest_neighbor(const TspSolution* solution)
{
    const TspInstance* instance = solution->instance;
    const long number_of_nodes = get_number_of_nodes(instance);
    const double* const  edge_cost_array = get_edge_cost_array(instance);
    const long start = rand() % number_of_nodes;

    int* tour = solution->tour;
    long visited = 1;
    double current_cost = 0.0;
    int current = tour[0];
    SWAP(tour[0], tour[start]);

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

        current_cost += edge_cost_array[current * number_of_nodes + tour[best_index]];
        SWAP(tour[visited], tour[best_index]);
        current = tour[visited];
        visited++;
    }

    *(double*)&solution->cost = current_cost;

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
