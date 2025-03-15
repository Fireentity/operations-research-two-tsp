#include <chrono.h>
#include <variable_neighborhood_search.h>
#include <c_util.h>
#include <float.h>
#include <nearest_neighbor.h>
#include <stdlib.h>
#include <string.h>
#include <tsp_math_util.h>

union TspExtendedAlgorithms
{
    VariableNeighborhoodSearch* variable_neighborhood_search;
};

static void two_opt(const int starting_node,
                      int* tour,
                      const int number_of_nodes,
                      const double* edge_cost_array,
                      double* cost)
{
    bool improved = true;

    // Continue iterating until no improvement is possible
    while (improved)
    {
        improved = false;
        for (int i = 1; i < number_of_nodes - 1; i++)
        {
            // Iterate over possible end indices for the segment
            for (int k = i + 1; k < number_of_nodes; k++)
            {

                const int edge_to_remove[] = {i, k};
                const double delta = compute_n_opt_cost(2, tour, edge_to_remove, edge_cost_array, number_of_nodes);
                // Skip if no improvement
                if (delta >= 0) continue;

                // Apply the improvement
                *cost += delta;
                compute_n_opt_move(2, tour, edge_to_remove);
                improved = true;

                // Break inner loop - restart with new tour
                break;
            }
            if (improved)
                break; // Break out of the outer loop as well to restart the process
        }
    }
}

static void kick(const int starting_node,
                      int* tour,
                      const int number_of_nodes,
                      const double* edge_cost_array,
                      double* cost)
{
    int r = (rand() % (number_of_nodes - min + 1)) + number_of_nodes;
    const int edge_to_remove[] = {i, k};
    const double delta = compute_n_opt_cost(2, tour, edge_to_remove, edge_cost_array, number_of_nodes);
}

static void solve(const TspAlgorithm* tsp_algorithm,
                  int* tour,
                  const int number_of_nodes,
                  const double* edge_cost_array,
                  double* cost)
{
}

const TspAlgorithm* init_vns(const int kick_repetition, const double time_limit)
{
    const VariableNeighborhoodSearch vns = {
        .kick_repetition = kick_repetition,
        .time_limit = time_limit,
    };
    const TspExtendedAlgorithms extended_algorithms = {
        .variable_neighborhood_search = malloc_from_stack(&vns, sizeof(vns))
    };
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .extended_algorithms = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
