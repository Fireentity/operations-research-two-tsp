#include <c_util.h>
#include <nearest_neighbor.h>
#include <float.h>
#include <math.h>
#include <plot_util.h>
#include <tsp_math_util.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <time_limiter.h>
#include "algorithm.c"

union TspExtendedAlgorithms {
    NearestNeighbor *nearest_neighbor;
};

static void free_this(const TspAlgorithm *self) {
    if (!self) return;
    if (self->extended) {
        if (self->extended->nearest_neighbor) free(self->extended->nearest_neighbor);
        free(self->extended);
    }
    free((void *) self);
}

static void nearest_neighbor_tour(const int starting_node,
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

static void solve(const TspAlgorithm *tsp_algorithm,
                  int *tour,
                  const int number_of_nodes,
                  const double *edge_cost_array,
                  double *cost) {
    double costs[number_of_nodes];
    const NearestNeighbor *nearest_neighbor = tsp_algorithm->extended->nearest_neighbor;
    const double time_limit = nearest_neighbor->time_limit;
    const TimeLimiter *time_limiter = init_time_limiter(time_limit);
    time_limiter->start(time_limiter);
    double best_cost = DBL_MAX;
    int best_tour[number_of_nodes];
    int starting_nodes[number_of_nodes];

    copy_int_array(tour, starting_nodes, number_of_nodes);
    shuffle_int_array(starting_nodes, number_of_nodes);

    int iteration = 0;
    do {
        nearest_neighbor_tour(starting_nodes[iteration], tour, number_of_nodes, edge_cost_array, cost);
        *cost += two_opt(tour, number_of_nodes, edge_cost_array, time_limiter);
        if (*cost < best_cost) {
            copy_int_array(tour, best_tour, number_of_nodes);
            best_cost = *cost;
        }
        costs[iteration] = *cost;
        iteration++;
    } while (!time_limiter->is_time_over(time_limiter) && iteration < number_of_nodes);

    copy_int_array(best_tour, tour, number_of_nodes);
    *cost = best_cost;
    plot_costs_evolution(costs, iteration, "neighbor_cost_eval.png");
    time_limiter->free(time_limiter);
}

const TspAlgorithm *init_nearest_neighbor(const double time_limit, const TspInstance *instance) {
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
