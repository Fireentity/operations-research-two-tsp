#include <stdio.h>
#include "algorithm_runner.h"
#include "cmd_options.h"
#include "tsp_instance.h"
#include "logger.h"
#include "c_util.h"

int main(const int argc, const char *argv[]) {
    CmdOptions *options = cmd_options_create_defaults();

    const ParsingResult *res =
            cmd_options_load(options, argc - 1, argv + 1);

    if (res->state != PARSE_SUCCESS) {
        if (res->state == PARSE_HELP) {
            cmd_options_destroy(options);
            return 0;
        }
        fprintf(stderr, "%s\n", res->error_message);
        cmd_options_destroy(options);
        return 1;
    }

#ifndef DISABLE_VERBOSE
    logger_set_verbosity(options->verbosity);
#endif

    if_verbose(VERBOSE_DEBUG,
               "--- Options ---\n"
               "Mode:        %s\n"
               "Input file:  %s\n"
               "Config file: %s\n"
               "Verbosity:   %u\n"
               "Plot path:   %s\n"
               "Nodes:       %u\n"
               "Seed:        %d\n"
               "Area:        %d,%d (side %u)\n"
               "Time limit:  %.2f\n"
               "--------------\n",
               (options->tsp.mode == TSP_INPUT_MODE_FILE ? "FILE" : "RANDOM"),
               options->tsp.input_file ? options->tsp.input_file : "(none)",
               options->config_file ? options->config_file : "(none)",
               options->verbosity,
               options->plots_path ? options->plots_path : "./",
               options->tsp.number_of_nodes,
               options->tsp.seed,
               options->tsp.generation_area.x_square,
               options->tsp.generation_area.y_square,
               options->tsp.generation_area.square_side,
               options->tsp.time_limit
    );

    TspInstance *instance = NULL;

    if (options->tsp.mode == TSP_INPUT_MODE_FILE) {
        const TspError err = tsp_instance_load_from_file(options->tsp.input_file, &instance);
        if (err != TSP_OK) {
            fprintf(stderr, "Failed to load instance: %s\n", tsp_error_to_string(err));
            cmd_options_destroy(options);
            return 1;
        }
    } else {
        instance = tsp_instance_create_random(
            (int) options->tsp.number_of_nodes,
            options->tsp.seed,
            (TspGenerationArea)
        {
            .
            x_square = options->tsp.generation_area.x_square,
            .
            y_square = options->tsp.generation_area.y_square,
            .
            square_side = options->tsp.generation_area.square_side
        }
        )
        ;
    }

    run_selected_algorithms(instance, options);

    tsp_instance_destroy(instance);
    cmd_options_destroy(options);
    return 0;
}