#include <stdio.h>
#include "algorithm_runner.h"
#include "cmd_options.h"
#include "c_util.h"
#include "logger.h"
#include "nearest_neighbor.h"


int main(const int argc, const char* argv[]) {
    CmdOptions* cmd_options = init_cmd_options();
    const ParsingResult* parsing_result = parse_application_options(cmd_options, argc - 1, argv + 1);

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
#ifndef DISABLE_VERBOSE
    logger_set_verbosity(cmd_options->verbosity);
#endif
    if_verbose(VERBOSE_INFO, "Parsing successful...\n");
    if_verbose(VERBOSE_DEBUG,
               "--- Parsed Options Dump ---\n"
               "  [General]\n"
               "    Verbosity:   %d\n"
               "  [TSP Instance]\n"
               "    Nodes:       %u\n"
               "    Seed:        %u\n"
               "    Time Limit:  %.2f s\n"
               "  [Algorithms Enabled]\n"
               "    VNS:         %s (Kicks: %u, N-Opt: %u)\n"
               "    NN:          %s\n"
               "    Tabu Search: %s (Tenure: %u, Stagnation: %u)\n"
               "    GRASP:       %s (P1: %.2f, P2: %.2f)\n"
               "-----------------------------\n",
               cmd_options->verbosity,
               cmd_options->tsp.number_of_nodes,
               cmd_options->tsp.seed,
               cmd_options->tsp.time_limit,
               cmd_options->variable_neighborhood_search ? "YES" : "NO",
               cmd_options->vns_params.kick_repetitions,
               cmd_options->vns_params.n_opt,
               cmd_options->nearest_neighbor ? "YES" : "NO",
               cmd_options->tabu_search ? "YES" : "NO",
               cmd_options->tabu_params.tenure,
               cmd_options->tabu_params.max_stagnation,
               cmd_options->grasp ? "YES" : "NO",
               cmd_options->grasp_params.p1,
               cmd_options->grasp_params.p2
    );

    const TspInstance* instance = init_random_tsp_instance(
        (int)cmd_options->tsp.number_of_nodes,
        cmd_options->tsp.seed,
        (TspGenerationArea){
            .square_side = cmd_options->tsp.generation_area.square_side,
            .x_square = cmd_options->tsp.generation_area.x_square,
            .y_square = cmd_options->tsp.generation_area.y_square,
        });

    run_selected_algorithms(instance, cmd_options);
    if_verbose(VERBOSE_INFO, "Algorithms executed, cleaning up...\n");
    // Cleanup
    instance->free(instance);
    free_cmd_option(cmd_options);
    return 0;
}
