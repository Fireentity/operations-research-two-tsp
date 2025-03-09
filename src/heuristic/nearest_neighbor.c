#include <float.h>
#include <stdlib.h>

#include "nearest_neighbor.h"


void solve_with_nearest_neighbor(
        unsigned long *tour,
        unsigned long number_of_nodes,
        const double *edge_cost_array,
        double *cost
) {

    const unsigned long start = rand() % number_of_nodes;
    unsigned long visited = 1;
    unsigned long current = tour[0]; //TODO non capisco troppo bene qua perche current è questo a poi viene subito swappato con start

    // Start from a random node
    SWAP(tour[0], tour[start]);

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

void two_opt(
        unsigned long *tour,
        const double *edge_cost_array,
        unsigned long number_of_nodes,
        double *cost
) {
    double best_cost = *cost;

    // Try reversing segments of the tour to find a better solution
    for (unsigned long i = 1; i < number_of_nodes - 1; i++) {
        for (unsigned long k = i + 1; k < number_of_nodes; k++) {
            //TODO verifica se è giusto il two_opt
            reverse_segment(tour, i, k);
            double new_cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
            if (new_cost < best_cost) {
                best_cost = new_cost;
                *cost = best_cost;
            } else {
                reverse_segment(tour, i, k); // Revert change if no improvement
            }
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