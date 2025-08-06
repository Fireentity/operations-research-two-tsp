#include "grasp.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "algorithms.h"
#include "tsp_algorithm.h"
#include "tsp_instance.h"
#include "tsp_math_util.h"
#include "tsp_solution.h"

#define NUMBER_OF_NODES 3

void dummy_add_cost(const CostsPlotter* self, double cost)
{
}

void dummy_plot(const CostsPlotter* self, const char* file_name)
{
}

void dummy_plot_free(const CostsPlotter* self)
{
}

const CostsPlotter dummy_plotter = {
    .add_cost = dummy_add_cost,
    .plot = dummy_plot,
    .free = dummy_plot_free,
};

// Helper function to create a simple edge cost matrix for testing
void create_simple_edge_costs(double* edge_costs, const int num_nodes)
{
    // Creating a simple fully connected matrix with random costs for simplicity
    for (int i = 0; i < num_nodes; i++)
    {
        for (int j = 0; j < num_nodes; j++)
        {
            if (i != j)
            {
                edge_costs[i * num_nodes + j] = (double)(rand() % 100 + 1); // Random costs between 1 and 100
            }
        }
    }
}

// Test for a basic case with 5 nodes for GRASP
void test_grasp_algorithm_basic_case()
{
    double cost = 0.0;
    const TspInstance* instance = init_random_tsp_instance(
        NUMBER_OF_NODES,
        42,
        (TspGenerationArea){
            .square_side = 100,
            .x_square = 0,
            .y_square = 0,
        });
    const TspSolution* solution = init_solution(instance); // Include the last node to close the tour
    const double* edge_costs = init_edge_cost_array(instance->get_nodes(instance),NUMBER_OF_NODES);
    const int* tour = solution->get_tour(solution);
    const double time_limit = 60.0; // 60 seconds time limit
    const TspAlgorithm* grasp_algorithm = init_grasp(time_limit,0.30,0.30);

    // Apply the GRASP algorithm
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    grasp_algorithm->solve(grasp_algorithm, (int*)tour, NUMBER_OF_NODES, edge_costs, &cost, &mutex, &dummy_plotter);

    // Verify the tour is completed and cost is updated
    assert(tour[0] == tour[NUMBER_OF_NODES]); // Ensure it's a closed tour
    assert(cost > 0); // Ensure a valid cost is calculated

    // Free the resources
    grasp_algorithm->free(grasp_algorithm);
}

// Test for boundary conditions, starting node out of range
void test_grasp_algorithm_invalid_starting_node()
{
    double cost = 0.0;
    double edge_costs[NUMBER_OF_NODES * NUMBER_OF_NODES];
    const TspInstance* instance = init_random_tsp_instance(
        5,
        42,
        (TspGenerationArea){
            .square_side = 100,
            .x_square = 0,
            .y_square = 0,
        });
    const TspSolution* solution = init_solution(instance); // Include the last node to close the tour
    const int* tour = solution->get_tour(solution);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    create_simple_edge_costs(edge_costs, NUMBER_OF_NODES);

    // Test for out-of-bounds starting node (e.g., node 4 for a 3-node problem)
    const int invalid_starting_node = 4;
    grasp_nearest_neighbor_tour(invalid_starting_node, (int*)tour, NUMBER_OF_NODES, edge_costs, &cost, 0.5, 0.3, 0.2);

    // Ensure a valid cost and tour are computed, even with the invalid starting node
    assert(tour[0] == tour[NUMBER_OF_NODES]); // Ensure it's a closed tour
    assert(cost > 0); // Ensure a valid cost is calculated
}

int main()
{
    test_grasp_algorithm_basic_case();
    test_grasp_algorithm_invalid_starting_node();
    puts("All tests passed!");
    return 0;
}
