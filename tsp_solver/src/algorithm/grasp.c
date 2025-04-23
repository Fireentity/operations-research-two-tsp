#include "grasp.h"
#include <float.h>
#include <algorithms.h>
#include "c_util.h"
#include <stdlib.h>
#include <time_limiter.h>
#include <constants.h>
#include <costs_plotter.h>
#include <nearest_neighbor.h>
#include <stdio.h>
#include <math.h>

union TspExtendedAlgorithms
{
    Grasp* grasp;
};

// The improvement function for GRASP performs the iterative improvement using NN + 2‑opt.
static void improve(const TspAlgorithm* tsp_algorithm,
                    int* tour,
                    const int number_of_nodes,
                    const double* edge_cost_array,
                    double* cost,
                    pthread_mutex_t* mutex)
{
    // Initialize the time limiter and the cost plotter.
    const double time_limit = tsp_algorithm->extended->grasp->time_limit;
    const TimeLimiter* time_limiter = init_time_limiter(time_limit);
    time_limiter->start(time_limiter);
    const CostsPlotter* plotter = init_plotter(number_of_nodes);

    // Work on a local copy of the tour.
    int current_tour[number_of_nodes + 1];
    int best_tour[number_of_nodes + 1];
    double current_cost;

    // Thread-safe copy of the input tour into current_tour.
    WITH_MUTEX(mutex,
               memcpy(current_tour, tour, (number_of_nodes + 1) * sizeof(int));
    );

    // Prepare an array of starting nodes from the initial tour and shuffle it.
    int starting_nodes[number_of_nodes];
    memcpy(starting_nodes, tour, number_of_nodes * sizeof(int));
    shuffle_int_array(starting_nodes, number_of_nodes);

    // Initialize best_cost to a high value.
    double best_cost = DBL_MAX;
    int iteration = 0;
    // Main loop: try each starting node or run until the time limit expires.
    while (!time_limiter->is_time_over(time_limiter) && iteration < number_of_nodes)
    {
        // Build a NN solution starting from starting_nodes[iteration].
        nearest_neighbor_tour(
            starting_nodes[iteration],
            current_tour,
            number_of_nodes,
            edge_cost_array,
            &current_cost);
        // Improve the solution using 2‑opt and update the cost.
        current_cost += two_opt(current_tour, number_of_nodes, edge_cost_array, time_limiter, EPSILON);
        // Record the current cost for plotting.
        plotter->add_cost(plotter, current_cost);
        // If the current solution is better than the best found so far, update best_tour and best_cost.
        if (current_cost < best_cost - EPSILON)
        {
            best_cost = current_cost;
            memcpy(best_tour, current_tour, (number_of_nodes + 1) * sizeof(int));
        }
        iteration++;
    }

    // Thread-safe update of the shared tour and cost if a better solution was found.
    if (best_cost < *cost)
    {
        WITH_MUTEX(mutex,
                   memcpy(tour, best_tour, (number_of_nodes + 1) * sizeof(int));
                   *cost = best_cost;
        );
    }
    // Plot the cost progression.
    plotter->plot_costs(plotter, "GRASP-costs.png");

    // Cleanup resources.
    time_limiter->free(time_limiter);
    plotter->free(plotter);
}

// The solve function for GRASP creates an initial solution with nearest neighbor
// and then calls improve to further optimize the tour.
static void solve(const TspAlgorithm* tsp_algorithm,
                  int* tour,
                  const int number_of_nodes,
                  const double* edge_cost_array,
                  double* cost,
                  pthread_mutex_t* mutex)
{
    // Create the initial tour in a thread-safe manner.

    WITH_MUTEX(mutex,
               grasp_nearest_neighbor_tour(
                   rand() % number_of_nodes,
                   tour,
                   number_of_nodes,
                   edge_cost_array,
                   cost,
                   tsp_algorithm->extended->grasp->p1,
                   tsp_algorithm->extended->grasp->p2,
                   tsp_algorithm->extended->grasp->p3);
    );
    // Improve the tour using the GRASP iterative improvement.
    improve(tsp_algorithm, tour, number_of_nodes, edge_cost_array, cost, mutex);
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
        .improve = improve,
        .free = free_this,
        .extended = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
