#include "ts_test.h"
#include <assert.h>
#include <stdio.h>
#include "tabu_search.h"
#include "tsp_algorithm.h"
#include "tsp_instance.h"
#include "tsp_solution.h"
#include "costs_plotter.h"

#define NUMBER_OF_NODES 10

// --- Dummy Plotter for testing ---
static void dummy_add_cost(const CostsPlotter* self, double cost) { (void)self; (void)cost; }
static void dummy_plot(const CostsPlotter* self, const char* file_name) { (void)self; (void)file_name; }
static void dummy_plot_free(const CostsPlotter* self) { (void)self; }

static const CostsPlotter dummy_plotter = {
    .add_cost = dummy_add_cost,
    .plot = dummy_plot,
    .free = dummy_plot_free,
};
// --- End Dummy Plotter ---

void run_ts_tests(void)
{
    printf("--- Running Tabu Search Algorithm Tests ---\n");
    const TspInstance* instance = init_random_tsp_instance(
        NUMBER_OF_NODES,
        42,
        (TspGenerationArea){
            .square_side = 100,
            .x_square = 0,
            .y_square = 0,
        });

    const TspSolution* solution = init_solution(instance);
    const double time_limit = 1.0;
    const int tenure = 5;
    const int max_stagnation = 50;

    const TspAlgorithm* ts_algorithm = init_tabu(tenure, max_stagnation, time_limit);

    ts_algorithm->solve(ts_algorithm, instance, solution, &dummy_plotter);

    const double cost = solution->get_cost(solution);
    int tour[NUMBER_OF_NODES + 1];
    solution->get_tour_copy(solution, tour);

    assert(tour[0] == tour[NUMBER_OF_NODES]);
    assert(cost > 0);

    ts_algorithm->free(ts_algorithm);
    solution->free(solution);
    instance->free(instance);
    printf("Tabu Search test passed.\n");
}