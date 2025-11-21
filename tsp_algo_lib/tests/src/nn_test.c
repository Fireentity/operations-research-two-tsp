#include <assert.h>
#include <stdio.h>
#include "nearest_neighbor.h"
#include "tsp_algorithm.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "costs_plotter.h" // Per il dummy plotter

#define NUMBER_OF_NODES 10

static void dummy_add_cost(const CostsPlotter *self, double cost) {
    (void) self;
    (void) cost;
}

static void dummy_plot(const CostsPlotter *self, const char *file_name) {
    (void) self;
    (void) file_name;
}

static void dummy_plot_free(const CostsPlotter *self) { (void) self; }

static const CostsPlotter dummy_plotter = {
    .add_cost = dummy_add_cost,
    .plot = dummy_plot,
    .free = dummy_plot_free,
};

void run_nn_tests(void) {
    printf("--- Running NN Algorithm Tests ---\n");
    const TspInstance *instance = init_random_tsp_instance(
        NUMBER_OF_NODES,
        42,
        (TspGenerationArea)
    {
        .
        square_side = 100,
        .
        x_square = 0,
        .
        y_square = 0,
    }
    )
    ;

    const TspSolution *solution = init_solution(instance);
    const double time_limit = 1.0;

    const TspAlgorithm *nn_algorithm = init_nearest_neighbor(time_limit);

    nn_algorithm->solve(nn_algorithm, instance, solution, &dummy_plotter);

    double cost = solution->get_cost(solution);
    int tour[NUMBER_OF_NODES + 1];
    solution->get_tour_copy(solution, tour);

    assert(tour[0] == tour[NUMBER_OF_NODES]);
    assert(cost > 0);

    nn_algorithm->free(nn_algorithm);
    solution->free(solution);
    instance->free(instance);
    printf("NN Algorithm test passed.\n");
}