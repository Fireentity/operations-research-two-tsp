#include <chrono.h>
#include <variable_neighborhood_search.h>
#include <c_util.h>
#include <nearest_neighbor.h>
#include <time_limiter.h>
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
 * @param tour            Array representing the current tour.
 * @param number_of_nodes Total number of nodes in the tour.
 * @param edge_cost_array Flattened 2D array containing edge costs.
 * @param cost            Pointer to the current tour cost to be updated.
 * @param time_limiter    Maximum allowed time for optimization, in seconds. If exceeded, the function terminates early.
 */
static double two_opt(int* tour,
                      const int number_of_nodes,
                      const double* edge_cost_array,
                      double* cost,
                      const TimeLimiter* time_limiter)
{
    // Iterate over possible starting indices for a two-opt move
    for (int i = 1; i < number_of_nodes - 1; i++)
    {
        if (time_limiter->is_time_over(time_limiter))
        {
            return 1;
        }


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
            compute_n_opt_move(2, tour, edge_to_remove, number_of_nodes);
            return delta;
        }
    }

    return 1;
}

static void optimize_with_two_opt(int* tour,
                          const int number_of_nodes,
                          const double* edge_cost_array,
                          double* cost,
                          const TimeLimiter* time_limiter)
{
    bool improved = true; // Flag to track if any improvement was made

    // Continue iterating until no further improvements are possible
    while (improved)
    {
        improved = false;
        if (two_opt(tour, number_of_nodes, edge_cost_array, cost, time_limiter))
        {
            improved= true;
        }

        if (time_limiter->is_time_over(time_limiter))
        {
            return;
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
    int edges_to_remove[3];

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

    //TODO salvare l'istanza migliore
    const int kick_repetition = tsp_algorithm->extended_algorithms->variable_neighborhood_search->kick_repetition;
    const int time_limit = tsp_algorithm->extended_algorithms->variable_neighborhood_search->time_limit;
    const TimeLimiter * time_limiter = init_time_limiter(time_limit);
    time_limiter->start(time_limiter);

    while (!time_limiter->is_time_over(time_limiter))
    {
        optimize_with_two_opt(tour, number_of_nodes, edge_cost_array, cost, time_limiter);
        for (int i = 0; i < kick_repetition; i++)
        {
            kick(tour, number_of_nodes, edge_cost_array, cost);
        }
    }

    time_limiter->free(time_limiter);
}

static void free_this(const TspAlgorithm* self)
{
    free(self->extended_algorithms->variable_neighborhood_search);
    free((void*)self);
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
        .free = free_this,
        .extended_algorithms = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
