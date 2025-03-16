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

/**
 * @brief Performs a two-opt optimization on the tour.
 *
 * Iteratively examines pairs of edges to find a two-opt move that reduces the tour cost.
 * When a beneficial move is found, the tour is updated and the process restarts.
 *
 * @param starting_node   Starting node of the tour (unused in this implementation).
 * @param tour            Array representing the current tour.
 * @param number_of_nodes Total number of nodes in the tour.
 * @param edge_cost_array Flattened 2D array containing edge costs.
 * @param cost            Pointer to the current tour cost to be updated.
 */
static void two_opt(const int starting_node,
                    int* tour,
                    const int number_of_nodes,
                    const double* edge_cost_array,
                    double* cost)
{
    bool improved = true;  // Flag to track if any improvement was made

    // Continue iterating until no further improvements are possible
    while (improved)
    {
        improved = false;
        // Iterate over possible starting indices for a two-opt move
        for (int i = 1; i < number_of_nodes - 1; i++)
        {
            // Iterate over possible end indices for the segment to be reversed
            for (int k = i + 1; k < number_of_nodes; k++)
            {
                // Define the endpoints of the segment to remove
                const int edge_to_remove[] = {i, k};
                // Compute the cost difference for the proposed two-opt move
                const double delta = compute_n_opt_cost(2, tour, edge_to_remove, edge_cost_array, number_of_nodes);
                // If the move does not improve the tour cost, skip it
                if (delta >= 0)
                    continue;

                // Update the tour cost with the improvement
                *cost += delta;
                // Apply the two-opt move to modify the tour
                compute_n_opt_move(2, tour, edge_to_remove);
                improved = true;

                // Break the inner loop to restart the search with the updated tour
                break;
            }
            if (improved)
                break; // Break the outer loop as well to restart the process
        }
    }
}


/**
 * @brief Performs a kick move on the tour by applying an n-opt operation.
 *
 * This function randomly selects a set of non-contiguous edges to remove from the tour,
 * computes the resulting cost change for a specific n-opt move, updates the overall cost,
 * and then applies the move to modify the tour configuration.
 *
 * @param tour             Array representing the current tour.
 * @param number_of_nodes  Total number of nodes in the tour.
 * @param edge_cost_array  Matrix of edge costs (flattened 2D array).
 * @param cost             Pointer to the current tour cost to be updated.
 */
static void kick(int* tour,
                 const int number_of_nodes,
                 const double* edge_cost_array,
                 double* cost)
{
    // Array to store indices of edges to remove.
    int edges_to_remove[4];

    // Compute the number of edges to remove.
    const int number_of_edges_to_remove = sizeof(edges_to_remove) / sizeof(edges_to_remove[0]);

    // Randomly select non-contiguous edges for removal.
    rand_k_non_contiguous(0, number_of_nodes, number_of_edges_to_remove, edges_to_remove);

    // Compute the cost change of the 3-opt move and update the total cost.
    *cost += compute_n_opt_cost(3, tour, edges_to_remove, edge_cost_array, number_of_nodes);

    // Apply the n-opt move to update the tour configuration.
    compute_n_opt_move(number_of_edges_to_remove, tour, edges_to_remove, number_of_nodes);
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
