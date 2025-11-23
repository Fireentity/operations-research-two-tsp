#include <stdio.h>
#include "algorithm_runner.h"
#include "cmd_options.h"
#include "tsp_instance.h"
#include "logger.h"
#include "c_util.h"

/**
 * @brief Configures the global logger based on command line options.
 */
static void setup_logging(const CmdOptions *options) {
#ifndef DISABLE_VERBOSE
    logger_set_verbosity(options->verbosity);
#endif
}

/**
 * @brief Prints the current configuration to the log if verbosity allows.
 */
static void print_configuration(const CmdOptions *options) {
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
}

/**
 * @brief Loads a TSP instance from a file or generates a random one based on options.
 * * @return TspInstance* Pointer to the created instance, or NULL on error.
 */
static TspInstance *create_tsp_instance(const CmdOptions *options) {
    TspInstance *instance = NULL;
    if (options->tsp.mode == TSP_INPUT_MODE_FILE) {
        const TspError err = tsp_instance_load_from_file(&instance, options->tsp.input_file);

        if (err != TSP_OK) {
            fprintf(stderr, "Failed to load instance: %s\n", tsp_error_to_string(err));
            return NULL;
        }
    }
    if (options->tsp.mode == TSP_INPUT_MODE_RANDOM) {
        //Random Generation Mode
        // Construct the area struct cleaner for readability
        const TspGenerationArea area = {
            .x_square = options->tsp.generation_area.x_square,
            .y_square = options->tsp.generation_area.y_square,
            .square_side = options->tsp.generation_area.square_side
        };
        instance = tsp_instance_create_random(options->tsp.number_of_nodes, options->tsp.seed, area);
    }


    return instance;
}

int main(const int argc, const char *argv[]) {
    // Parse Command Line Arguments
    CmdOptions *options = cmd_options_create_defaults();
    const ParsingResult *res = cmd_options_load(options, argc - 1, argv + 1); // remove program name

    if (res->state != PARSE_SUCCESS) {
        int exit_code = 1;
        if (res->state == PARSE_HELP) {
            exit_code = 0;
        } else {
            fprintf(stderr, "%s\n", res->error_message);
        }
        cmd_options_destroy(options);
        return exit_code;
    }

    // Setup Logging
    setup_logging(options);
    print_configuration(options);

    // Create/Load Instance
    TspInstance *instance = create_tsp_instance(options);
    if (!instance) {
        // Error message already printed in create_tsp_instance for file errors
        cmd_options_destroy(options);
        return 1;
    }

    // Run Algorithms
    run_selected_algorithms(instance, options);

    // Cleanup
    tsp_instance_destroy(instance);
    cmd_options_destroy(options);

    return 0;
}
