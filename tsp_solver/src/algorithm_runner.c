#include "algorithm_runner.h"
#include <stdio.h>
#include <linux/limits.h>
#include <string.h>
#include "c_util.h"
#include "logger.h"
#include "variable_neighborhood_search.h"
#include "nearest_neighbor.h"
#include "tabu_search.h"
#include "grasp.h"
#include "tsp_solution.h"
#include "costs_plotter.h"
#include "plot_util.h"
#include "algorithm_plotter.h"

typedef struct {
    const TspAlgorithm* algorithm;
    const char* name;
    const char* plot_file;
    const char* costs_file;
} AlgorithmTask;

/**
 * @brief Static helper function to run and report on a single algorithm.
 */
static void execute_and_report(const TspAlgorithm* algorithm,
                               const TspInstance* instance,
                               const char* algorithm_name,
                               const char* plot_file,
                               const char* costs_file) {
    if_verbose(VERBOSE_DEBUG, "  Initializing plotter for %s...\n", algorithm_name);
    const CostsPlotter* plotter = init_plotter(instance->get_number_of_nodes(instance));

    if_verbose(VERBOSE_DEBUG, "  Initializing solution for %s...\n", algorithm_name);
    const TspSolution* solution = init_solution(instance);

    if_verbose(VERBOSE_DEBUG, "  Calling solve() for %s...\n", algorithm_name);
    solution->solve(solution, algorithm, plotter);

    if_verbose(VERBOSE_DEBUG, "  Plotting tour to %s.\n", plot_file);
    const int nr_nodes = instance->get_number_of_nodes(instance);
    int tour_buffer[nr_nodes + 1];
    solution->get_tour_copy(solution, tour_buffer);
    plot_tour(tour_buffer,
              instance->get_number_of_nodes(instance),
              instance->get_nodes(instance),
              plot_file);

    if_verbose(VERBOSE_DEBUG, "  Plotting costs to %s.\n", costs_file);
    plotter->plot(plotter, costs_file);

    if_verbose(VERBOSE_INFO, "%s solution: %lf\n", algorithm_name, solution->get_cost(solution));

    // Resources created in this function are freed here
    if_verbose(VERBOSE_DEBUG, "  Freeing solution for %s.\n", algorithm_name);
    solution->free(solution);

    if_verbose(VERBOSE_DEBUG, "  Freeing plotter for %s.\n", algorithm_name);
    plotter->free(plotter);
}

// Implementation of the public function
void run_selected_algorithms(const TspInstance* instance, const CmdOptions* cmd_options) {
    // TODO hardcoded magic number
    AlgorithmTask tasks[4]; // Max number of algorithms
    int task_count = 0;

    if_verbose(VERBOSE_DEBUG, "Configuring algorithm run queue...\n");

    if (cmd_options->nn_params.enable) {
        if_verbose(VERBOSE_DEBUG, "  Queueing NN (t=%.2fs)\n", cmd_options->tsp.time_limit);
        tasks[task_count++] = (AlgorithmTask){
            .algorithm = init_nearest_neighbor(cmd_options->tsp.time_limit),
            .name = "NN",
            .plot_file = cmd_options->nn_params.plot_file,
            .costs_file = cmd_options->nn_params.cost_file
        };
    }
    if (cmd_options->vns_params.enable) {
        if_verbose(VERBOSE_DEBUG, "  Queueing VNS (k=%d, n_opt=%d, t=%.2fs)\n",
                   (int)cmd_options->vns_params.kick_repetitions,
                   (int)cmd_options->vns_params.n_opt,
                   cmd_options->tsp.time_limit);
        tasks[task_count++] = (AlgorithmTask){
            .algorithm = init_vns((int)cmd_options->vns_params.kick_repetitions,
                                  (int)cmd_options->vns_params.n_opt,
                                  (int)cmd_options->tsp.time_limit),
            .name = "VNS",
            .plot_file = cmd_options->vns_params.plot_file,
            .costs_file = cmd_options->vns_params.cost_file
        };
    }
    if (cmd_options->tabu_params.enable) {
        if_verbose(VERBOSE_DEBUG, "  Queueing TS (tenure=%d, stagnation=%d, t=%.2fs)\n",
                   (int)cmd_options->tabu_params.tenure,
                   (int)cmd_options->tabu_params.max_stagnation,
                   cmd_options->tsp.time_limit);
        tasks[task_count++] = (AlgorithmTask){
            .algorithm = init_tabu((int)cmd_options->tabu_params.tenure,
                                   (int)cmd_options->tabu_params.max_stagnation,
                                   cmd_options->tsp.time_limit),
            .name = "TS",
            .plot_file = cmd_options->tabu_params.plot_file,
            .costs_file = cmd_options->tabu_params.cost_file
        };
    }
    if (cmd_options->grasp_params.enable) {
        if_verbose(VERBOSE_DEBUG, "  Queueing GRASP (p1=%.2f, p2=%.2f, t=%.2fs)\n",
                   cmd_options->grasp_params.p1,
                   cmd_options->grasp_params.p2,
                   cmd_options->tsp.time_limit);
        tasks[task_count++] = (AlgorithmTask){
            .algorithm = init_grasp(cmd_options->tsp.time_limit,
                                    cmd_options->grasp_params.p1,
                                    cmd_options->grasp_params.p2),
            .name = "GRASP",
            .plot_file = cmd_options->grasp_params.plot_file,
            .costs_file = cmd_options->grasp_params.cost_file
        };
    }

    if_verbose(VERBOSE_INFO, "Running %d algorithm(s)...\n", task_count);

    char full_plot_path[PATH_MAX];
    char full_costs_path[PATH_MAX];
    for (int i = 0; i < task_count; i++) {
        if_verbose(VERBOSE_INFO, "--- Running %s ---\n", tasks[i].name);

        if (cmd_options->plot_path && strlen(cmd_options->plot_path) > 0) {
            snprintf(full_plot_path, PATH_MAX, "%s%s",
                     cmd_options->plot_path,
                     tasks[i].plot_file);
        } else {
            snprintf(full_plot_path, PATH_MAX, "%s", tasks[i].plot_file);
        }

        if (cmd_options->plot_path && strlen(cmd_options->plot_path) > 0) {
            snprintf(full_costs_path, PATH_MAX, "%s%s",
                     cmd_options->plot_path,
                     tasks[i].costs_file);
        } else {
            snprintf(full_costs_path, PATH_MAX, "%s", tasks[i].costs_file);
        }

        execute_and_report(tasks[i].algorithm,
                           instance,
                           tasks[i].name,
                           full_plot_path,
                           full_costs_path);

        if_verbose(VERBOSE_DEBUG, "Freeing algorithm %s.\n", tasks[i].name);
        tasks[i].algorithm->free(tasks[i].algorithm);
    }

    if_verbose(VERBOSE_INFO, "All algorithms finished.\n");

}
