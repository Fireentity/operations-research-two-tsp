#include <variable_neighborhood_search.h>
#include <c_util.h>
#include <nearest_neighbor.h>
#include <time_limiter.h>
#include <tsp_math_util.h>
#include "algorithm.c"

union TspExtendedAlgorithms
{
    VariableNeighborhoodSearch* variable_neighborhood_search;
};

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
 */
static double kick(int tour[],
                   const int number_of_nodes,
                   const double* edge_cost_array)
{
    // Array to store indices of edges to remove.
    int edges_to_remove[3];

    // Compute the number of edges to remove.
    const int number_of_edges_to_remove = sizeof(edges_to_remove) / sizeof(edges_to_remove[0]);

    // Randomly select non-contiguous edges for removal.
    rand_k_non_contiguous(0, number_of_nodes-1, number_of_edges_to_remove, edges_to_remove);

    // Compute the cost change of the 3-opt move and update the total cost.
    const double delta = compute_n_opt_cost(3, tour, edges_to_remove, edge_cost_array, number_of_nodes);

    // Apply the n-opt move to update the tour configuration.
    compute_n_opt_move(number_of_edges_to_remove, tour, edges_to_remove, number_of_nodes);

    return delta;
}

static void solve(const TspAlgorithm* tsp_algorithm,
                  int tour[],
                  const int number_of_nodes,
                  const double edge_cost_array[],
                  double* cost)
{
    shuffle_int_array(tour, number_of_nodes);
    tour[number_of_nodes] = tour[0];

    const int kick_repetition = tsp_algorithm->extended->variable_neighborhood_search->kick_repetition;
    const int time_limit = tsp_algorithm->extended->variable_neighborhood_search->time_limit;
    const TimeLimiter* time_limiter = init_time_limiter(time_limit);
    int best_tour[number_of_nodes + 1];
    double best_cost = *cost;
    copy_int_array(tour, best_tour, number_of_nodes + 1);
    time_limiter->start(time_limiter);

    bool is_time_over = false;
    while (!is_time_over)
    {
        *cost += two_opt(tour, number_of_nodes, edge_cost_array, time_limiter);
        if (*cost < best_cost)
        {
            copy_int_array(tour, best_tour, number_of_nodes + 1);
            best_cost = *cost;
        }

        is_time_over = time_limiter->is_time_over(time_limiter);
        if (!is_time_over)
        {
            for (int i = 0; i < kick_repetition; i++)
            {
                *cost += kick(tour, number_of_nodes, edge_cost_array);
            }
        }
    }

    *cost = best_cost;
    copy_int_array(best_tour, tour, number_of_nodes + 1);
    time_limiter->free(time_limiter);
}

static void free_this(const TspAlgorithm* self)
{
    free(self->extended->variable_neighborhood_search);
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
        .extended = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
