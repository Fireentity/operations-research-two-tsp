#include <stdio.h>
#include "algorithm_plotter.h"
#include "cmd_options.h"
#include "c_util.h"
#include "grasp.h"
#include "nearest_neighbor.h"
#include "plot_util.h"
#include "tabu_search.h"
#include "tsp_solution.h"
#include "variable_neighborhood_search.h"


/**
 * @brief Runs all selected algorithms on the given instance.
 * (Updated to use the new nested CmdOptions struct)
 */
void run_algorithms(const TspInstance* instance, const CmdOptions* cmd_options) {
    if (cmd_options->variable_neighborhood_search) {
        const CostsPlotter* plotter = init_plotter(instance->get_number_of_nodes(instance));
        const TspSolution* solution = init_solution(instance);

        const TspAlgorithm* algorithm = init_vns((int)cmd_options->vns_params.kick_repetitions,
                                                 (int)cmd_options->vns_params.n_opt,
                                                 (int)cmd_options->tsp.time_limit);

        solution->solve(solution, algorithm, plotter);
        plot_tour(solution->get_tour(solution),
                  instance->get_number_of_nodes(instance),
                  instance->get_nodes(instance),
                  "VNS-plot.png");
        printf("VNS solution: %lf\n", solution->get_cost(solution));
        solution->free(solution);
        algorithm->free(algorithm);
    }
    if (cmd_options->nearest_neighbor) {
        const CostsPlotter* plotter = init_plotter(instance->get_number_of_nodes(instance));
        const TspSolution* solution = init_solution(instance);

        const TspAlgorithm* algorithm = init_nearest_neighbor(cmd_options->tsp.time_limit);

        solution->solve(solution, algorithm, plotter);
        plot_tour(solution->get_tour(solution),
                  instance->get_number_of_nodes(instance),
                  instance->get_nodes(instance),
                  "NN-plot.png");
        printf("NN solution: %lf\n", solution->get_cost(solution));
        solution->free(solution);
        algorithm->free(algorithm);
    }
    if (cmd_options->tabu_search) {
        const CostsPlotter* plotter = init_plotter(instance->get_number_of_nodes(instance));
        const TspSolution* solution = init_solution(instance);

        // --- UPDATED to use nested tabu_params and tsp structs ---
        const TspAlgorithm* algorithm = init_tabu((int)cmd_options->tabu_params.tenure,
                                                  (int)cmd_options->tabu_params.max_stagnation,
                                                  cmd_options->tsp.time_limit);
        solution->solve(solution, algorithm, plotter);
        plot_tour(solution->get_tour(solution),
                  instance->get_number_of_nodes(instance),
                  instance->get_nodes(instance),
                  "TS-plot.png");
        printf("TS solution: %lf\n", solution->get_cost(solution));
        solution->free(solution);
        algorithm->free(algorithm);
    }
    if (cmd_options->grasp) {
        const CostsPlotter* plotter = init_plotter(instance->get_number_of_nodes(instance));
        const TspSolution* solution = init_solution(instance);

        // --- UPDATED to use nested grasp_params and tsp structs ---
        const TspAlgorithm* algorithm = init_grasp(cmd_options->tsp.time_limit,
                                                   cmd_options->grasp_params.p1,
                                                   cmd_options->grasp_params.p2);
        solution->solve(solution, algorithm, plotter);
        plot_tour(solution->get_tour(solution),
                  instance->get_number_of_nodes(instance),
                  instance->get_nodes(instance),
                  "GR-plot.png");
        printf("GR solution: %lf\n", solution->get_cost(solution));
        solution->free(solution);
        algorithm->free(algorithm);
    }
}

int main(const int argc, const char* argv[]) {
    CmdOptions* cmd_options = init_cmd_options();
    const ParsingResult* parsing_result = parse_application_options(cmd_options, argc-1, argv+1);
    // Check for parsing errors
    switch (parsing_result->state) {
        case PARSE_SUCCESS: break;
        case PARSE_HELP:
            free_cmd_option(cmd_options);
            return 0;;
        default:
            printf("%s", parsing_result->error_message);
            free_cmd_option(cmd_options);
            return 1;
    }


    const TspInstance* instance = init_random_tsp_instance((int)cmd_options->tsp.number_of_nodes,
                                                           cmd_options->tsp.seed,
                                                           (TspGenerationArea){
                                                               .square_side = cmd_options->tsp.generation_area.
                                                               square_side,
                                                               .x_square = cmd_options->tsp.generation_area.x_square,
                                                               .y_square = cmd_options->tsp.generation_area.y_square,
                                                           });
    if_verbose(cmd_options->verbosity, VERBOSE_INFO, "Parsing successful...\n");
    run_algorithms(instance, cmd_options);
    instance->free(instance);
    free_cmd_option(cmd_options);
    return 0;
}
