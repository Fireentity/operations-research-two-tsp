#include <time_limiter.h>
#include <tsp_math_util.h>
#include <constants.h>

/**
* @brief Performs a two-opt optimization on the tour.
 *
 * Iteratively examines pairs of edges to find a two-opt move that reduces the tour cost.
 * When a beneficial move is found, the tour is updated and the process restarts.
 *
 * @param tour            Array representing the current tour.
 * @param number_of_nodes Total number of nodes in the tour.
 * @param edge_cost_array Flattened 2D array containing edge costs.
 * @param time_limiter    Maximum allowed time for optimization, in seconds. If exceeded, the function terminates early.
 */
static inline double two_opt(int* tour,
                      const int number_of_nodes,
                      const double* edge_cost_array,
                      const TimeLimiter* time_limiter)
{
    double cost_improvement = 0;
    // Iterate over possible starting indices for a two-opt move
    int i = 0;
    while (i < number_of_nodes)
    {
        if (time_limiter->is_time_over(time_limiter))
        {
            return cost_improvement;
        }

        double delta = 0;

        // Iterate over possible end indices for the segment to be reversed
        for (int k = i + 2; k < number_of_nodes; k++)
        {
            if (i == 0 && k == number_of_nodes - 1) continue;
            // Define the endpoints of the segment to remove
            const int edge_to_remove[] = {i, k};
            // Compute the cost difference for the proposed two-opt move
            delta = compute_n_opt_cost(2, tour, edge_to_remove, edge_cost_array, number_of_nodes);
            // If the move does not improve the tour cost, skip it
            if (delta > -EPSILON)
                continue;

            cost_improvement += delta;
            // Apply the two-opt move to modify the tour
            compute_n_opt_move(2, tour, edge_to_remove, number_of_nodes);
            break;
        }

        i = delta < -EPSILON ? 0 : i + 1;
    }

    return cost_improvement;
}
