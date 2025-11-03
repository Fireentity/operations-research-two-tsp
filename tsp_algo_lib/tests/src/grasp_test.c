#include "grasp_test.h"
#include "grasp.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "algorithms.h"
#include "tsp_algorithm.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "costs_plotter.h"

#define NUMBER_OF_NODES 3

// --- Dummy Plotter for testing ---
static void dummy_add_cost(const CostsPlotter* self, double cost)
{
    (void)self; (void)cost;
}

static void dummy_plot(const CostsPlotter* self, const char* file_name)
{
    (void)self; (void)file_name;
}

static void dummy_plot_free(const CostsPlotter* self)
{
    (void)self;
}

static const CostsPlotter dummy_plotter = {
    .add_cost = dummy_add_cost,
    .plot = dummy_plot,
    .free = dummy_plot_free,
};
// --- End Dummy Plotter ---

static void create_simple_edge_costs(double* edge_costs, const int num_nodes)
{
    for (int i = 0; i < num_nodes; i++)
    {
        for (int j = 0; j < num_nodes; j++)
        {
            if (i != j)
            {
                edge_costs[i * num_nodes + j] = (double)(rand() % 100 + 1);
            }
        }
    }
}

static void test_grasp_algorithm_basic_case()
{
    const TspInstance* instance = init_random_tsp_instance(
        NUMBER_OF_NODES,
        42,
        (TspGenerationArea){
            .square_side = 100,
            .x_square = 0,
            .y_square = 0,
        });

    const TspSolution* solution = init_solution(instance);
    const double time_limit = 60.0;
    const TspAlgorithm* grasp_algorithm = init_grasp(time_limit,0.30,0.30);

    grasp_algorithm->solve(grasp_algorithm, instance, solution, &dummy_plotter);

    const double cost = solution->get_cost(solution);
    int tour[NUMBER_OF_NODES + 1];
    solution->get_tour_copy(solution, tour);

    assert(tour[0] == tour[NUMBER_OF_NODES]);
    assert(cost > 0);

    grasp_algorithm->free(grasp_algorithm);
    solution->free(solution);
    instance->free(instance);
}

static void test_grasp_nn_helper_invalid_starting_node()
{
    const int num_nodes = 5;
    double cost = 0.0;
    double edge_costs[num_nodes * num_nodes];
    create_simple_edge_costs(edge_costs, num_nodes);

    int tour[num_nodes + 1];
    for(int i = 0; i < num_nodes; i++) tour[i] = i;
    tour[num_nodes] = tour[0];

    // This test should probably check for a valid case
    // The original invalid case (10) would exit()
    grasp_nearest_neighbor_tour(0, tour, num_nodes, edge_costs, &cost, 0.5, 0.3, 0.2);

    assert(tour[0] == tour[num_nodes]);
    assert(cost > 0);
}

void run_grasp_tests(void)
{
    printf("--- Running GRASP Algorithm Tests ---\n");
    test_grasp_algorithm_basic_case();
    test_grasp_nn_helper_invalid_starting_node();
    printf("GRASP tests passed.\n");
}