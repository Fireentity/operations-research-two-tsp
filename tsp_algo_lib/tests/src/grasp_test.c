#include "grasp_test.h"
#include "grasp.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "costs_plotter.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "algorithms.h"

#define NUMBER_OF_NODES 3

static void dummy_add_cost(const CostsPlotter *self, double cost) {
    (void) self;
    (void) cost;
}

static void dummy_plot(const CostsPlotter *self, const char *file_name) {
    (void) self;
    (void) file_name;
}

static void dummy_plot_free(const CostsPlotter *self) {
    (void) self;
}

static const CostsPlotter dummy_plotter = {
    .add_cost = dummy_add_cost,
    .plot = dummy_plot,
    .free = dummy_plot_free,
};

static void create_simple_edge_costs(double *edge_costs, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (i != j)
                edge_costs[i * n + j] = (double) (rand() % 100 + 1);
}

static void test_grasp_algorithm_basic_case() {
    TspInstance *instance =
            tsp_instance_create_random(
                NUMBER_OF_NODES,
                42,
                (TspGenerationArea)
    {
        0, 0, 100
    }
    )
    ;

    TspSolution *sol = tsp_solution_create(instance);
    const TspAlgorithm *grasp = init_grasp(60.0, 0.30, 0.30);

    tsp_algorithm_solve(grasp, instance, sol, &dummy_plotter);

    double cost = tsp_solution_get_cost(sol);
    int tour[NUMBER_OF_NODES + 1];
    tsp_solution_get_tour_copy(sol, tour);

    assert(tour[0] == tour[NUMBER_OF_NODES]);
    assert(cost > 0);

    grasp->free(grasp);
    tsp_solution_free(sol);
    tsp_instance_destroy(instance);
}

static void test_grasp_nn_helper_basic_case() {
    const int n = 5;
    double edge_costs[n * n];
    create_simple_edge_costs(edge_costs, n);

    int tour[n + 1];
    for (int i = 0; i < n; i++) tour[i] = i;
    tour[n] = tour[0];

    double cost = 0.0;

    grasp_nearest_neighbor_tour(0, tour, n, edge_costs, &cost, 0.5, 0.3, 0.2);

    assert(tour[0] == tour[n]);
    assert(cost > 0);
}

void run_grasp_tests(void) {
    printf("--- Running GRASP Algorithm Tests ---\n");
    test_grasp_algorithm_basic_case();
    test_grasp_nn_helper_basic_case();
    printf("GRASP tests passed.\n");
}