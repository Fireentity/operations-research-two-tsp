#include "algorithm_runner.h"
#include "nearest_neighbor.h"
#include "variable_neighborhood_search.h"
#include "tabu_search.h"
#include "grasp.h"
#include "../include/plot_util.h"
#include "c_util.h"
#include "logger.h"
#include "constants.h"

#include <stdio.h>
#include <linux/limits.h>
#include <string.h>


static void execute_and_report(const TspAlgorithm *algo,
                               const TspInstance *instance,
                               const char *plot_file,
                               const char *costs_file) {
    // --- Cost Recorder ---
    CostRecorder *recorder = cost_recorder_create(RECORDER_INITIAL_CAPACITY);

    // --- Solution ---
    TspSolution *solution = tsp_solution_create(instance);

    // --- Run Algorithm ---
    tsp_algorithm_run(algo, instance, solution, recorder);
    // --- Extract Tour ---
    const int n = tsp_instance_get_num_nodes(instance);
    int *tour_buffer = malloc((n + 1) * sizeof(int));
    check_alloc(tour_buffer);

    tsp_solution_get_tour(solution, tour_buffer);
    const double cost = tsp_solution_get_cost(solution);

    // --- Plot Tour ---
    plot_tour(tour_buffer, n, tsp_instance_get_nodes(instance), plot_file);

    // --- TODO: integrate with new CostPlotter architecture ---
    // cost_reporter_export(&reporter, costs_file);
    plot_costs_evolution(cost_recorder_get_costs(recorder), cost_recorder_get_count(recorder), costs_file);

    if_verbose(VERBOSE_INFO, "%s solution: %lf\n", algo->name, cost);

    // --- Cleanup ---
    free(tour_buffer);
    cost_recorder_destroy(recorder);
    tsp_solution_destroy(solution);
    tsp_algorithm_destroy((TspAlgorithm *) algo);
}

void run_selected_algorithms(const TspInstance *instance, const CmdOptions *options) {
    char full_plot_path[PATH_MAX];
    char full_costs_path[PATH_MAX];

#define BUILD_PATHS(plot_fname, cost_fname) \
        if (options->plots_path && strlen(options->plots_path) > 0) { \
            join_path(full_plot_path, options->plots_path, plot_fname, PATH_MAX); \
            join_path(full_costs_path, options->plots_path, cost_fname, PATH_MAX); \
        } else { \
            snprintf(full_plot_path, PATH_MAX, "%s", plot_fname); \
            snprintf(full_costs_path, PATH_MAX, "%s", cost_fname); \
        }

    // --- NN ---
    if (options->nn_params.enable) {
        NNConfig cfg = {.time_limit = options->tsp.time_limit};
        TspAlgorithm algo = nn_create(cfg);
        BUILD_PATHS(options->nn_params.plot_file, options->nn_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

    // --- VNS ---
    if (options->vns_params.enable) {
        VNSConfig cfg = {
            .kick_repetition = (int) options->vns_params.kick_repetitions,
            .n_opt = (int) options->vns_params.n_opt,
            .time_limit = options->tsp.time_limit
        };
        TspAlgorithm algo = vns_create(cfg);
        BUILD_PATHS(options->vns_params.plot_file, options->vns_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

    // --- TABU ---
    if (options->tabu_params.enable) {
        TabuConfig cfg = {
            .tenure = (int) options->tabu_params.tenure,
            .max_stagnation = (int) options->tabu_params.max_stagnation,
            .time_limit = options->tsp.time_limit
        };
        TspAlgorithm algo = tabu_create(cfg);
        BUILD_PATHS(options->tabu_params.plot_file, options->tabu_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

    // --- GRASP ---
    if (options->grasp_params.enable) {
        GraspConfig cfg = {
            .p1 = options->grasp_params.p1,
            .p2 = options->grasp_params.p2,
            .time_limit = options->tsp.time_limit
        };
        TspAlgorithm algo = grasp_create(cfg);
        BUILD_PATHS(options->grasp_params.plot_file, options->grasp_params.cost_file);
        execute_and_report(&algo, instance, full_plot_path, full_costs_path);
    }

#undef BUILD_PATHS
}
