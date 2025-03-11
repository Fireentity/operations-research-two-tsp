#include <c_util.h>
#include <nearest_neighbor.h>
#include "extended_algorithm.c"
#include <chrono.h>
#include <float.h>
#include <tsp_math_util.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void two_opt(unsigned long* tour,
                    unsigned long number_of_nodes,
                    const double* edge_cost_array,
                    double* cost);

static void nearest_neighbor(unsigned long starting_node,
                             unsigned long* tour,
                             unsigned long number_of_nodes,
                             const double* edge_cost_array,
                             double* cost);

static void solve(const TspAlgorithm* tsp_algorithm,
           unsigned long* tour,
           const unsigned long number_of_nodes,
           const double* edge_cost_array,
           double* cost)
{
    double best_solution_cost = DBL_MAX;
    const double time_limit = tsp_algorithm->nearest_neighbor.time_limit;
    const double start = second();

    long starting_nodes[number_of_nodes];
    memcpy(starting_nodes, tour, sizeof(unsigned long) * number_of_nodes);
    SHUFFLE_ARRAY(starting_nodes, number_of_nodes);

    unsigned long incumbent_starting_node = starting_nodes[0];

    int iteration = 0;
    do
    {
        nearest_neighbor(starting_nodes[iteration],
                         tour,
                         number_of_nodes,
                         edge_cost_array,
                         cost);
        two_opt(tour,
                number_of_nodes,
                edge_cost_array,
                cost);

        if (*cost < best_solution_cost)
        {
            incumbent_starting_node = tour[0];
            best_solution_cost = *cost;
        }
        iteration++;
    }
    while (start - second() < time_limit && iteration < number_of_nodes);

    nearest_neighbor(incumbent_starting_node,
                     tour,
                     number_of_nodes,
                     edge_cost_array,
                     cost);
    two_opt(tour,
            number_of_nodes,
            edge_cost_array,
            cost);
}

const TspAlgorithm* init_nearest_neighbor(const double time_limit)
{
    TspAlgorithm* tsp_algorithm_ptr = malloc(sizeof(TspAlgorithm));
    check_alloc(tsp_algorithm_ptr);
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .nearest_neighbor = {
            .time_limit = time_limit
        },
    };
    memcpy(tsp_algorithm_ptr, &tsp_algorithm, sizeof(tsp_algorithm_ptr));

    return tsp_algorithm_ptr;
}

static void nearest_neighbor(const unsigned long starting_node,
                             unsigned long* tour,
                             const unsigned long number_of_nodes,
                             const double* edge_cost_array,
                             double* cost)
{
    if (starting_node > number_of_nodes)
    {
        printf("The starting node (%ld) cannot be greater than the number of nodes (%ld)",
               starting_node, number_of_nodes);
        exit(EXIT_FAILURE);
    }

    unsigned long visited = 1;

    // Start from the node in input
    SWAP(tour[0], tour[starting_node]);
    unsigned long current = tour[0];

    // Closing the tour
    tour[number_of_nodes] = tour[0];

    while (visited < number_of_nodes)
    {
        double best_cost = DBL_MAX;
        unsigned long best_index = visited;

        // Find the nearest unvisited node
        for (unsigned long i = visited; i < number_of_nodes; i++)
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

static void two_opt(unsigned long* tour,
                    const unsigned long number_of_nodes,
                    const double* edge_cost_array,
                    double* cost)
{
    bool improved = true;

    // Continue iterating until no improvement is possible
    while (improved)
    {
        improved = false;

        // Iterate over possible start indices for the segment to reverse
        for (unsigned long i = 1; i < number_of_nodes - 1; i++)
        {
            int found = 0;

            // Iterate over possible end indices for the segment
            for (unsigned long k = i + 1; k < number_of_nodes; k++)
            {
                // Identify the four nodes involved in the current potential 2-opt move
                unsigned long a = tour[i - 1]; // Node immediately before the segment
                unsigned long b = tour[i]; // First node of the segment
                unsigned long c = tour[k]; // Last node of the segment
                unsigned long d = tour[(k + 1) % number_of_nodes]; // Node immediately after the segment (wraps around)

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
