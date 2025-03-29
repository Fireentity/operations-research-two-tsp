#include "grasp.h"
#include <algorithms.h>
#include "c_util.h"
#include <stdlib.h>
#include <float.h>
#include <time_limiter.h>
#include <constants.h>
#include <costs_plotter.h>
#include <nearest_neighbor.h>
#include <tsp_math_util.h>
#include <stdio.h>
#include <math.h>

union TspExtendedAlgorithms
{
    Grasp* grasp;
};

typedef struct
{
    int rcl_size;
    int elements[];
    int current_rcl_elements;
} RCL;

RCL* init_RCL(const int rcl_size)
{
    const int elements[rcl_size] = {0};
    malloc_from_stack(elements, sizeof(elements[0]) * rcl_size);

    const RCL rcl = {
        .elements = elements,
        .current_rcl_elements = 0,
        .rcl_size = rcl_size,
    };

    return malloc_from_stack(&rcl, sizeof(rcl));
}

void insert_element(RCL* rcl, const int node)
{
    const int index = rcl->current_rcl_elements == rcl->rcl_size ? rand()%rcl->current_rcl_elements : rcl->current_rcl_elements;
    rcl->elements[index] = node;
    rcl->current_rcl_elements++;
}

void fill_rcl(const int number_of_visited_nodes,
                 const int number_of_nodes,
                 const double edge_cost_array[],
                 const int current_node_index,
                 RCL* restricted_candidates,
                 const double threshold)
{
    for (int i = number_of_visited_nodes; i < number_of_nodes; i++)
    {
        const double edge_cost = edge_cost_array[current_node_index*number_of_nodes + i];
        if (edge_cost< threshold)
        {
            insert_element(restricted_candidates, i);
        }
    }
}

void grasp_tour(const int starting_node,
                int* tour,
                const int number_of_nodes,
                const double* edge_cost_array,
                double* cost)
{
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

    RCL* rcl = init_RCL(number_of_nodes);

    while (visited < number_of_nodes) {
        fill_rcl(visited, number_of_nodes, edge_cost_array, current, rcl, );
        // Move the best found node to the next position in the tour
        swap_int(tour, visited, best_index);
        current = tour[visited];
        visited++;
    }

    // Compute the total cost of the generated tour
    *cost = calculate_tour_cost(tour, number_of_nodes, edge_cost_array);
}

// Main solve function for GRASP
static void solve(const TspAlgorithm* tsp_algorithm,
                  int* tour,
                  const int number_of_nodes,
                  const double* edge_cost_array,
                  double* cost)
{
    // Start the timer
    const int time_limit = tsp_algorithm->extended->grasp->time_limit;
    const TimeLimiter* time_limiter = init_time_limiter(time_limit);
    time_limiter->start(time_limiter);

    // Start the costs plotter
    const CostsPlotter* plotter = init_plotter(number_of_nodes);

    // Work on my own tour.
    int current_tour[number_of_nodes + 1];
    int best_tour[number_of_nodes + 1];
    double current_cost;

    // Initialize current_tour by copying the input tour.
    //TODO make it thread safe
    memcpy(current_tour, tour, (number_of_nodes + 1) * sizeof(int));

    // Prepare an array of starting nodes from the initial tour and shuffle them.
    int starting_nodes[number_of_nodes];
    memcpy(starting_nodes, tour, number_of_nodes * sizeof(int));
    shuffle_int_array(starting_nodes, number_of_nodes);

    // Initialize best_cost to a large value.
    double best_cost = DBL_MAX;

    int iteration = 0;
    // Main loop: try each starting node or run until the time limit expires.
    while (!time_limiter->is_time_over(time_limiter) && iteration < number_of_nodes)
    {
        // Build a NN solution starting from starting_nodes[iteration].
        nearest_neighbor_tour(starting_nodes[iteration], current_tour, number_of_nodes, edge_cost_array, &current_cost);
        // Improve the solution with 2-opt and update the cost.
        current_cost += two_opt(current_tour, number_of_nodes, edge_cost_array, time_limiter);
        plotter->add_cost(plotter, current_cost);
        // If the current solution is better than the best found so far, update best_tour and best_cost.
        if (current_cost < best_cost - EPSILON)
        {
            best_cost = current_cost;
            memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
        }
        iteration++;
    }

    // Copy the best found tour back to the provided output and update cost.
    // TODO make it thread safe
    if (best_cost < *cost)
    {
        memcpy(tour, best_tour, (number_of_nodes + 1) * sizeof(int));
        *cost = best_cost;
    }
    plotter->plot_costs(plotter, "NN-costs.png");
    // Cleanup.
    time_limiter->free(time_limiter);
    plotter->free(plotter);
}

// Free function for TspAlgorithm
static void free_this(const TspAlgorithm* self)
{
    if (!self) return;

    if (self->extended)
    {
        if (self->extended->grasp)
        {
            free(self->extended->grasp);
        }
        free(self->extended);
    }
    free((void*)self);
}

// Initialize the GRASP algorithm with the time limit parameter
const TspAlgorithm* init_grasp(const double time_limit)
{
    const Grasp grasp = {
        .time_limit = time_limit,
    };
    const TspExtendedAlgorithms extended_algorithms = {
        .grasp = malloc_from_stack(&grasp, sizeof(grasp))
    };
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
