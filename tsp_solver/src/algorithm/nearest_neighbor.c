#include <c_util.h>
#include <nearest_neighbor.h>
#include <chrono.h>
#include <float.h>
#include <tsp_math_util.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

union TspExtendedAlgorithms
{
    NearestNeighbor* nearest_neighbor;
};


static void two_opt(int* tour,
                    int number_of_nodes,
                    const double* edge_cost_array,
                    double* cost,
                    double time_limit);

static void apply_nearest_neighbor(int starting_node,
                                   int* tour,
                                   int number_of_nodes,
                                   const double* edge_cost_array,
                                   double* cost);

static void solve(const TspAlgorithm* tsp_algorithm,
                  int* tour,
                  const int number_of_nodes,
                  const double* edge_cost_array,
                  double* cost)
{
    const NearestNeighbor* nearest_neighbor = tsp_algorithm->extended_algorithms->nearest_neighbor;
    double best_solution_cost = DBL_MAX;
    const double time_limit = nearest_neighbor->time_limit;
    const double start = second();

    int starting_nodes[number_of_nodes];
    memcpy(starting_nodes, tour, sizeof(int) * number_of_nodes);
    SHUFFLE_ARRAY(starting_nodes, number_of_nodes);

    int incumbent_starting_node = starting_nodes[0];

    int iteration = 0;
    do
    {
        apply_nearest_neighbor(starting_nodes[iteration],
                               tour,
                               number_of_nodes,
                               edge_cost_array,
                               cost);
        two_opt(tour,
                number_of_nodes,
                edge_cost_array,
                cost,
                time_limit);

        if (*cost < best_solution_cost)
        {
            incumbent_starting_node = tour[0];
            best_solution_cost = *cost;
        }
        iteration++;
    }
    while (start - second() < time_limit && iteration < number_of_nodes);

    apply_nearest_neighbor(incumbent_starting_node,
                           tour,
                           number_of_nodes,
                           edge_cost_array,
                           cost);
    two_opt(tour,
            number_of_nodes,
            edge_cost_array,
            cost,
            time_limit);
}

const TspAlgorithm* init_nearest_neighbor(const double time_limit)
{
    NearestNeighbor nearest_neighbor = {
        .time_limit = time_limit
    };

    TspExtendedAlgorithms extended_algorithms = {
        .nearest_neighbor = MALLOC_FROM_STACK(nearest_neighbor)
    };

    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .extended_algorithms = MALLOC_FROM_STACK(extended_algorithms),
    };

    return MALLOC_FROM_STACK(tsp_algorithm);
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
    SWAP(tour[0], tour[starting_node]);
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
        SWAP(tour[visited], tour[best_index]);
        current = tour[visited];
        visited++;
    }

    // Compute the total cost of the generated tour
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
}

static void two_opt(int* tour,
                    const int number_of_nodes,
                    const double* edge_cost_array,
                    double* cost,
                    const double time_limit)
{
    const double start_time = second();
    bool improved = true;

    // Continue iterating until no improvement is possible
    while (improved)
    {
        improved = false;
        EXECUTE_AFTER(start_time, time_limit, return);

        // Iterate over possible start indices for the segment to reverse
        for (int i = 1; i < number_of_nodes - 1; i++)
        {
            int found = 0;

            // Iterate over possible end indices for the segment
            for (int k = i + 1; k < number_of_nodes; k++)
            {
                // Identify the four nodes involved in the current potential 2-opt move
                int a = tour[i - 1]; // Node immediately before the segment
                int b = tour[i]; // First node of the segment
                int c = tour[k]; // Last node of the segment
                int d = tour[(k + 1) % number_of_nodes]; // Node immediately after the segment (wraps around)

                // Calculate the cost difference (delta) if the segment between i and k is reversed
                // Delta = (cost of new edges: (a, c) + (b, d)) - (cost of old edges: (a, b) + (c, d))
                const double delta = edge_cost_array[a * number_of_nodes + c] +
                    edge_cost_array[b * number_of_nodes + d] -
                    edge_cost_array[a * number_of_nodes + b] -
                    edge_cost_array[c * number_of_nodes + d];

                // If delta is negative, the new tour is shorter (improvement)
                if (delta < 0)
                {
                    // Reverse the segment between i and k to apply the improvement
                    REVERSE_ARRAY(tour, i, k);

                    // Update the overall tour cost.
                    *cost += delta;
                    improved = true;
                    found = 1;
                    break; // Break out of the inner loop to restart with the updated tour
                }
            }
            if (found)
                break; // Break out of the outer loop as well to restart the process
        }
    }
}
