#include <math.h>
#include <tsp_solution.h>
#include <feasibility_result.h>
#include <stdlib.h>
#include <string.h>
#include <tsp_math_util.h>
#include <tsp_algorithm.h>
#include <c_util.h>

#define EPSILON 1e-10

struct TspSolutionState
{
    double cost;
    int* const tour;
    const TspInstance* const instance;
};

const int* get_tour(const TspSolution* solution) { return solution->state->tour; }

double compute_cost(const TspSolution* solution)
{
    const TspInstance* instance = solution->state->instance;
    return calculate_tour_cost(
            solution->state->tour,
            instance->get_number_of_nodes(instance),
            instance->get_edge_cost_array(instance)
    );
}

FeasibilityResult is_feasible(const TspSolution* solution)
{
    const TspInstance* instance = solution->state->instance;
    const int* const tour = solution->state->tour;
    const int number_of_nodes = solution->state->instance->get_number_of_nodes(instance);
    int counter[number_of_nodes];
    memset(counter, 0, number_of_nodes * sizeof(counter[0]));

    for (int i = 0; i < number_of_nodes; i++)
    {
        // Checks that each value in the solution array is a valid index in the TSP problem's node array
        if (tour[i] < 0 || tour[i] > number_of_nodes - 1)
        {
            return UNINITIALIZED_ENTRY;
        }

        counter[tour[i]]++;

        // Ensures each index appears exactly once; if an index appears more than once or not at all, the TSP solution is infeasible
        // This is because in a valid TSP tour, each node must be visited exactly once to form a Hamiltonian cycle
        if (counter[tour[i]] != 1)
        {
            return DUPLICATED_ENTRY;
        }
    }

    // Recalculate the total cost of the current solution
    const double calculated_cost = compute_cost(solution);

    // Check if the stored cost is approximately equal to the recalculated cost within an epsilon margin
    // This accounts for floating-point precision errors
    if (fabs(solution->state->cost - calculated_cost) > EPSILON)
    {
        return NON_MATCHING_COST;
    }

    return FEASIBLE;
}

FeasibilityResult solve(const TspSolution* solution, const TspAlgorithm* tsp_algorithm)
{
    const TspInstance* instance = solution->state->instance;
    tsp_algorithm->solve(tsp_algorithm,
                         solution->state->tour,
                         instance->get_number_of_nodes(instance),
                         instance->get_edge_cost_array(instance),
                         &solution->state->cost);
    printf("%lf\n", solution->state->cost);
    return is_feasible(solution);
}

static void free_this(const TspSolution* solution)
{
    if (!solution || !solution->state->tour)
    {
        return;
    }
    free(solution->state->tour);
    free((void*)solution);
}

int* init_tour(const int number_of_nodes)
{
    int* tour = calloc(number_of_nodes + 1, sizeof(int));
    check_alloc(tour);
    for (int i = 0; i < number_of_nodes; i++)
        tour[i] = i;
    tour[number_of_nodes] = tour[0];
    return tour;
}

TspSolution* init_solution(const TspInstance* instance)
{
    const int number_of_nodes = instance->get_number_of_nodes(instance);

    int* tour = init_tour(number_of_nodes);

    const TspSolutionState state = {
            .cost = calculate_tour_cost(tour, number_of_nodes, instance->get_edge_cost_array(instance)),
            .tour = tour,
            .instance = instance
    };
    const TspSolution solution = {
            .free = free_this,
            .is_feasible = is_feasible,
            .solve = solve,
            .get_tour = get_tour,
            .state = malloc_from_stack(&state, sizeof(state))
    };
    return malloc_from_stack(&solution, sizeof(solution));
}
