#include <float.h>
#include <stdlib.h>
#include "nearest_neighbor.h"
#include "constants.h"


void solve_with_nearest_neighbor(
        unsigned long *tour,
        unsigned long number_of_nodes,
        const double *edge_cost_array,
        double *cost
) {
    const unsigned long start = rand() % number_of_nodes;
    unsigned long visited = 1;

    // Start from a random node
    SWAP(tour[0], tour[start]);
    unsigned long current = tour[0];

    // Closing the tour
    tour[number_of_nodes] = tour[0];

    while (visited < number_of_nodes) {
        double best_cost = DBL_MAX;
        unsigned long best_index = visited;

        // Find the nearest unvisited node
        for (unsigned long i = visited; i < number_of_nodes; i++) {
            const double cost_candidate = edge_cost_array[current * number_of_nodes + tour[i]];
            if (cost_candidate < best_cost) {
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

void two_opt(unsigned long *tour, unsigned long number_of_nodes, const double *edge_cost_array, double *cost) {

    bool improved = true;

    // Continue iterating until no improvement is possible
    while (improved) {
        improved = false;

        // Iterate over possible start indices for the segment to reverse
        for (unsigned long i = 1; i < number_of_nodes - 1; i++) {
            int found = 0;

            // Iterate over possible end indices for the segment
            for (unsigned long k = i + 1; k < number_of_nodes; k++) {

                // Identify the four nodes involved in the current potential 2-opt move
                unsigned long a = tour[i - 1];              // Node immediately before the segment
                unsigned long b = tour[i];                  // First node of the segment
                unsigned long c = tour[k];                  // Last node of the segment
                unsigned long d = tour[(k + 1) % number_of_nodes]; // Node immediately after the segment (wraps around)

                // Calculate the cost difference (delta) if the segment between i and k is reversed
                // Delta = (cost of new edges: (a, c) + (b, d)) - (cost of old edges: (a, b) + (c, d))
                double delta = edge_cost_array[a * number_of_nodes + c] +
                               edge_cost_array[b * number_of_nodes + d] -
                               edge_cost_array[a * number_of_nodes + b] -
                               edge_cost_array[c * number_of_nodes + d];

                // If delta is negative, the new tour is shorter (improvement)
                if (delta < -EPSILON) {

                    // Reverse the segment between i and k to apply the improvement
                    reverse_segment(tour, i, k);

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


static void reverse_segment(unsigned long *tour, unsigned long i, unsigned long k) {
    while (i < k) {
        SWAP(tour[i], tour[k]);
        i++;
        k--;
    }
}

void solve_with_nearest_neighbor_and_two_opt(unsigned long *tour, unsigned long number_of_nodes,
                                             const double *edge_cost_array, double *cost) {
    solve_with_nearest_neighbor(tour,number_of_nodes,edge_cost_array,cost);
    two_opt(tour,number_of_nodes,edge_cost_array,cost);
}
