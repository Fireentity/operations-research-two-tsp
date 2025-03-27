#include "algorithms.h"
#include <tsp_math_util.h>
#include <constants.h>
#include <float.h>

#include "c_util.h"


inline double two_opt(int* tour,
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

void nearest_neighbor_tour(const int starting_node,
                                  int *tour,
                                  const int number_of_nodes,
                                  const double *edge_cost_array,
                                  double *cost) {
    if (starting_node > number_of_nodes) {
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

    while (visited < number_of_nodes) {
        double best_cost = DBL_MAX;
        int best_index = visited;

        // Find the nearest unvisited node
        for (int i = visited; i < number_of_nodes; i++) {
            const double cost_candidate = edge_cost_array[current * number_of_nodes + tour[i]];
            if (cost_candidate < best_cost) {
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



