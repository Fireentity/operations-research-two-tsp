#include <c_util.h>
#include <nearest_neighbor.h>
#include <chrono.h>
#include <constants.h>
#include <float.h>
#include <math.h>
#include <plot_util.h>
#include <tsp_math_util.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <time_limiter.h>

union TspExtendedAlgorithms
{
    NearestNeighbor* nearest_neighbor;
};

static void free_this(const TspAlgorithm* self)
{
    free(self->extended->nearest_neighbor);
    free((void*)self);
}

static void nearest_neighbor_tour(const int starting_node,
                                       int* tour,
                                       const int number_of_nodes,
                                       const double* edge_cost_array,
                                       double* cost)
{
    if (starting_node > number_of_nodes)
    {
        printf("The starting node (%d) cannot be greater than the number of nodes (%d)",
               starting_node, number_of_nodes);
        exit(EXIT_FAILURE);
    }

    int visited = 1;

    // Start from the node in input
    swap_int(tour, 0, starting_node);
    int current = tour[0];

    // Closing the tour
    tour[number_of_nodes] = tour[0];

    while (visited < number_of_nodes)
    {
        double best_cost = DBL_MAX;
        int best_index = visited;

        // Find the nearest unvisited node
        for (int i = visited; i < number_of_nodes; i++)
        {
            const double cost_candidate = edge_cost_array[current * number_of_nodes + tour[i]];
            if (cost_candidate < best_cost)
            {
                best_cost = cost_candidate;
                best_index = i;
            }
        }
        // Move the best found node to the next position in the tour
        swap_int(tour, visited, best_index);
        current = tour[visited];
        visited++;
    }

    // Compute the total cost of the generated tour
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
}

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
static double two_opt(int* tour,
                      const int number_of_nodes,
                      const double* edge_cost_array,
                      const TimeLimiter* time_limiter)
{
    // Iterate over possible starting indices for a two-opt move
    for (int i = 0; i < number_of_nodes; i++)
    {
        if (time_limiter->is_time_over(time_limiter))
        {
            return 0;
        }

        // Iterate over possible end indices for the segment to be reversed
        for (int k = i + 2; k < number_of_nodes; k++)
        {
            if (i == 0 && k == number_of_nodes - 1) continue;
            // Define the endpoints of the segment to remove
            const int edge_to_remove[] = {i, k};
            // Compute the cost difference for the proposed two-opt move
            const double delta = compute_n_opt_cost(2, tour, edge_to_remove, edge_cost_array, number_of_nodes);
            // If the move does not improve the tour cost, skip it
            if (delta > -EPSILON)
                continue;

            // Apply the two-opt move to modify the tour
            compute_n_opt_move(2, tour, edge_to_remove, number_of_nodes);
            return delta;
        }
    }

    return 0;
}

static double optimize_with_two_opt(int* tour,
                                    const int number_of_nodes,
                                    const double* edge_cost_array,
                                    const TimeLimiter* time_limiter)
{
    double delta = 0;
    double cost_improvement = 0;
    // Continue iterating until no further improvements are possible
    do
    {
        delta = two_opt(tour, number_of_nodes, edge_cost_array, time_limiter);
        cost_improvement += delta;

        if (time_limiter->is_time_over(time_limiter))
        {
            return cost_improvement;
        }
    }
    while (delta < 0);

    return cost_improvement;
}

static void solve(const TspAlgorithm* tsp_algorithm,
                  int* tour,
                  const int number_of_nodes,
                  const double* edge_cost_array,
                  double* cost)
{
    const NearestNeighbor* nearest_neighbor = tsp_algorithm->extended->nearest_neighbor;
    double best_solution_cost = DBL_MAX;
    const double time_limit = nearest_neighbor->time_limit;
    const TimeLimiter* time_limiter = init_time_limiter(time_limit);

    int starting_nodes[number_of_nodes];
    int incumbent_starting_node = starting_nodes[0];

    memcpy(starting_nodes, tour, sizeof(int) * number_of_nodes);
    shuffle_int_array(starting_nodes, number_of_nodes);
    time_limiter->start(time_limiter);

    int iteration = 0;
    do
    {
        nearest_neighbor_tour(starting_nodes[iteration], tour, number_of_nodes, edge_cost_array, cost);
        *cost += optimize_with_two_opt(tour, number_of_nodes, edge_cost_array, time_limiter);

        if (*cost < best_solution_cost)
        {
            incumbent_starting_node = tour[0];
            best_solution_cost = *cost;
        }
        iteration++;
    }
    while (time_limiter->is_time_over(time_limiter) && iteration < number_of_nodes);

    nearest_neighbor_tour(incumbent_starting_node, tour, number_of_nodes, edge_cost_array, cost);
    *cost = optimize_with_two_opt(tour, number_of_nodes, edge_cost_array, time_limiter);

    time_limiter->free(time_limiter);
}

const TspAlgorithm* init_nearest_neighbor(const double time_limit, const TspInstance* instance)
{
    const NearestNeighbor nearest_neighbor = {
        .time_limit = time_limit,
        .instance = instance
    };

    const TspExtendedAlgorithms extended_algorithms = {
        .nearest_neighbor = malloc_from_stack(&nearest_neighbor, sizeof(nearest_neighbor))
    };

    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };

    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
