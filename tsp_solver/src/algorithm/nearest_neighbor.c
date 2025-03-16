#include <c_util.h>
#include <nearest_neighbor.h>
#include <chrono.h>
#include <float.h>
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
    free(self->extended_algorithms->nearest_neighbor);
    free((void*)self);
}

static void apply_nearest_neighbor(const int starting_node,
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
        for (int k = i + 2; k < number_of_nodes; k++)
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
        improved = two_opt(tour, number_of_nodes, edge_cost_array, cost, time_limiter) < 0;

        if (time_limiter->is_time_over(time_limiter))
        {
            return;
        }
    }
}

static void solve(const TspAlgorithm* tsp_algorithm,
                  int* tour,
                  const int number_of_nodes,
                  const double* edge_cost_array,
                  double* cost)
{
    const NearestNeighbor* nearest_neighbor = tsp_algorithm->extended_algorithms->nearest_neighbor;
    double best_solution_cost = DBL_MAX;
    const double time_limit = nearest_neighbor->time_limit;
    const TimeLimiter* time_limiter = init_time_limiter(time_limit);
    time_limiter->start(time_limiter);

    int starting_nodes[number_of_nodes];
    memcpy(starting_nodes, tour, sizeof(int) * number_of_nodes);
    shuffle_int_array(starting_nodes, number_of_nodes);

    int incumbent_starting_node = starting_nodes[0];

    int iteration = 0;
    do
    {
        apply_nearest_neighbor(starting_nodes[iteration], tour, number_of_nodes, edge_cost_array, cost);
        optimize_with_two_opt(tour, number_of_nodes, edge_cost_array, cost, time_limiter);

        if (*cost < best_solution_cost)
        {
            incumbent_starting_node = tour[0];
            best_solution_cost = *cost;
        }
        iteration++;
    }
    while (time_limiter->is_time_over(time_limiter) && iteration < number_of_nodes);

    apply_nearest_neighbor(incumbent_starting_node, tour, number_of_nodes, edge_cost_array, cost);
    optimize_with_two_opt(tour, number_of_nodes, edge_cost_array, cost, time_limiter);

    time_limiter->free(time_limiter);
}

const TspAlgorithm* init_nearest_neighbor(const double time_limit)
{
    const NearestNeighbor nearest_neighbor = {
        .time_limit = time_limit
    };

    const TspExtendedAlgorithms extended_algorithms = {
        .nearest_neighbor = malloc_from_stack(&nearest_neighbor, sizeof(nearest_neighbor))
    };

    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended_algorithms = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };

    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}