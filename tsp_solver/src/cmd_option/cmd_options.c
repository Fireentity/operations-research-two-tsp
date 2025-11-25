#include "cmd_options.h"
#include "cmd_option_internal.h"
#include "flag_parser.h"
#include "c_util.h" // for memdup
#include <stdlib.h>
#include <string.h>
#include "logger.h"

// TODO does not print
static const ParsingResult *validate_options(const CmdOptions *opt) {
    if_verbose(VERBOSE_DEBUG, "Starting configuration validation...\n");
    // --- 1. TSP Instance Validation ---
    if (opt->tsp.mode == TSP_INPUT_MODE_FILE) {
        if (opt->tsp.input_file == NULL || strlen(opt->tsp.input_file) == 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] Mode is FILE but no input file provided (--file).\n");
            return MISSING_MANDATORY_FLAG;
        }
    } else if (opt->tsp.mode == TSP_INPUT_MODE_RANDOM) {
        if (opt->tsp.number_of_nodes < 2) {
            if_verbose(VERBOSE_INFO, "[Config Error] Random generation requires at least 2 nodes.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->tsp.generation_area.square_side == 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] Generation area square side must be > 0.\n");
            return WRONG_VALUE_TYPE;
        }
    }
    if_verbose(VERBOSE_DEBUG, "TSP instance options validated.\n");

    // --- 2. Nearest Neighbor Validation ---
    if (opt->nn_params.enable) {
        if (opt->nn_params.time_limit < 0.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] NN: Time limit cannot be negative.\n");
            return WRONG_VALUE_TYPE;
        }
        if_verbose(VERBOSE_DEBUG, "NN options validated.\n");
    }

    // --- 3. VNS Validation ---
    if (opt->vns_params.enable) {
        if (opt->vns_params.kick_repetitions == 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] VNS: Kick repetitions must be > 0.\n");
            return USAGE_ERROR;
        }
        if (opt->vns_params.min_k < 2) {
            if_verbose(VERBOSE_INFO, "[Config Error] VNS: min_k must be >= 2 (e.g., 2-opt, 3-opt).\n");
            return WRONG_VALUE_TYPE;
        }

        if (opt->vns_params.max_k < 2) {
            if_verbose(VERBOSE_INFO, "[Config Error] VNS: max_k must be >= 2 (e.g., 2-opt, 3-opt).\n");
            return WRONG_VALUE_TYPE;
        }

        if (opt->vns_params.min_k > opt->vns_params.max_k) {
            if_verbose(VERBOSE_INFO, "[Config Error] VNS: min_k cannot be greater than max_k.\n");
            return WRONG_VALUE_TYPE;
        }

        if (opt->vns_params.max_stagnation <= 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] VNS: Max Stagnation must be positive.\n");
            return WRONG_VALUE_TYPE;
        }
        if_verbose(VERBOSE_DEBUG, "VNS options validated.\n");
    }

    // --- 4. Tabu Search Validation ---
    if (opt->tabu_params.enable) {
        if (opt->tabu_params.min_tenure == 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] Tabu: Min Tenure must be > 0.\n");
            return USAGE_ERROR;
        }
        if (opt->tabu_params.max_tenure == 0 || opt->tabu_params.min_tenure > opt->tabu_params.max_tenure) {
            if_verbose(VERBOSE_INFO, "[Config Error] Tabu: Max Tenure must be > 0 and >= min_tenure.\n");
            return USAGE_ERROR;
        }
        if (opt->tabu_params.max_stagnation == 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] Tabu: Max stagnation must be > 0.\n");
            return USAGE_ERROR;
        }
        if (opt->tabu_params.time_limit < 0.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] Tabu: Time limit cannot be negative.\n");
            return WRONG_VALUE_TYPE;
        }
        if_verbose(VERBOSE_DEBUG, "Tabu Search options validated.\n");
    }

    // --- 5. GRASP Validation ---
    if (opt->grasp_params.enable) {
        if (opt->grasp_params.p1 < 0.0 || opt->grasp_params.p1 > 1.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] GRASP: p1 (probability) must be between 0.0 and 1.0.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->grasp_params.p2 < 0.0 || opt->grasp_params.p2 > 1.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] GRASP: p2 must be between 0.0 and 1.0.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->grasp_params.time_limit < 0.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] GRASP: Time limit cannot be negative.\n");
            return WRONG_VALUE_TYPE;
        }
        if_verbose(VERBOSE_DEBUG, "GRASP options validated.\n");
    }

    // --- 6. General Warnings ---
    // Check if no algorithms are enabled
    if (!opt->nn_params.enable && !opt->vns_params.enable &&
        !opt->tabu_params.enable && !opt->grasp_params.enable) {
        if_verbose(VERBOSE_INFO, "[Warning] No algorithms enabled. The solver will exit after instance generation.\n");
    }

    if_verbose(VERBOSE_DEBUG, "Configuration validation completed successfully.\n");
    return SUCCESS;
}

typedef enum { OPT_INT, OPT_UINT, OPT_DOUBLE, OPT_BOOL, OPT_STRING } OptType;

typedef struct {
    const char *flag; // CLI flag name used to detect overrides
    OptType type; // type of the option
    void *final_ptr; // pointer into final options struct
    const void *ini_ptr; // pointer into ini-loaded temp struct
} OptionMapEntry;

static void merge_value(OptType type, void *dst, const void *src) {
    switch (type) {
        case OPT_INT:
            *(int *) dst = *(const int *) src;
            break;
        case OPT_UINT:
            *(unsigned int *) dst = *(const unsigned int *) src;
            break;
        case OPT_DOUBLE:
            *(double *) dst = *(const double *) src;
            break;
        case OPT_BOOL:
            *(bool *) dst = *(const bool *) src;
            break;
        case OPT_STRING:
            if (*(char **) dst) free(*(char **) dst);
            *(char **) dst = *(char * const*) src ? strdup(*(char * const*) src) : NULL;
            break;
    }
}

static void merge_ini_into_options(CmdOptions *final,
                                   const CmdOptions *ini,
                                   const FlagParser *cli) {
    const OptionMapEntry map[] = {
        // GENERAL
        {"--verbosity", OPT_UINT, &final->verbosity, &ini->verbosity},
        {"--plot-path", OPT_STRING, &final->plots_path, &ini->plots_path},

        // TSP
        {"--mode", OPT_INT, &final->tsp.mode, &ini->tsp.mode},
        {"--nodes", OPT_UINT, &final->tsp.number_of_nodes, &ini->tsp.number_of_nodes},
        {"--seed", OPT_INT, &final->tsp.seed, &ini->tsp.seed},
        {"--file", OPT_STRING, &final->tsp.input_file, &ini->tsp.input_file},

        {"--x-square", OPT_INT, &final->tsp.generation_area.x_square, &ini->tsp.generation_area.x_square},
        {"--y-square", OPT_INT, &final->tsp.generation_area.y_square, &ini->tsp.generation_area.y_square},
        {"--square-side", OPT_UINT, &final->tsp.generation_area.square_side, &ini->tsp.generation_area.square_side},

        // NN
        {"--nn", OPT_BOOL, &final->nn_params.enable, &ini->nn_params.enable},
        {"--nn-plot", OPT_STRING, &final->nn_params.plot_file, &ini->nn_params.plot_file},
        {"--nn-cost", OPT_STRING, &final->nn_params.cost_file, &ini->nn_params.cost_file},
        {"--nn-seconds", OPT_DOUBLE, &final->nn_params.time_limit, &ini->nn_params.time_limit},

        // VNS
        {"--vns", OPT_BOOL, &final->vns_params.enable, &ini->vns_params.enable},
        {"--vns-min-k", OPT_UINT, &final->vns_params.min_k, &ini->vns_params.min_k},
        {"--vns-max-k", OPT_UINT, &final->vns_params.max_k, &ini->vns_params.max_k},
        {"--vns-kik-reps", OPT_UINT, &final->vns_params.kick_repetitions, &ini->vns_params.kick_repetitions},
        {"--vns-stagnation", OPT_UINT, &final->vns_params.max_stagnation, &ini->vns_params.max_stagnation},
        {"--vns-plot", OPT_STRING, &final->vns_params.plot_file, &ini->vns_params.plot_file},
        {"--vns-cost", OPT_STRING, &final->vns_params.cost_file, &ini->vns_params.cost_file},
        {"--vns-seconds", OPT_DOUBLE, &final->vns_params.time_limit, &ini->vns_params.time_limit},

        // TABU
        {"--ts", OPT_BOOL, &final->tabu_params.enable, &ini->tabu_params.enable},
        {"--ts-min-tenure", OPT_UINT, &final->tabu_params.min_tenure, &ini->tabu_params.min_tenure},
        {"--ts-max-tenure", OPT_UINT, &final->tabu_params.max_tenure, &ini->tabu_params.max_tenure},
        {"--ts-stagnation", OPT_UINT, &final->tabu_params.max_stagnation, &ini->tabu_params.max_stagnation},
        {"--ts-plot", OPT_STRING, &final->tabu_params.plot_file, &ini->tabu_params.plot_file},
        {"--ts-cost", OPT_STRING, &final->tabu_params.cost_file, &ini->tabu_params.cost_file},
        {"--ts-seconds", OPT_DOUBLE, &final->tabu_params.time_limit, &ini->tabu_params.time_limit},

        // GRASP
        {"--grasp", OPT_BOOL, &final->grasp_params.enable, &ini->grasp_params.enable},
        {"--grasp-p1", OPT_DOUBLE, &final->grasp_params.p1, &ini->grasp_params.p1},
        {"--grasp-p2", OPT_DOUBLE, &final->grasp_params.p2, &ini->grasp_params.p2},
        {"--grasp-plot", OPT_STRING, &final->grasp_params.plot_file, &ini->grasp_params.plot_file},
        {"--grasp-cost", OPT_STRING, &final->grasp_params.cost_file, &ini->grasp_params.cost_file},
        {"--grasp-seconds", OPT_DOUBLE, &final->grasp_params.time_limit, &ini->grasp_params.time_limit},
    };

    const size_t count = sizeof(map) / sizeof(map[0]);

    for (size_t i = 0; i < count; i++) {
        const OptionMapEntry *e = &map[i];
        if (!flag_parser_is_visited(cli, e->flag)) {
            merge_value(e->type, e->final_ptr, e->ini_ptr);
        }
    }
}

const ParsingResult *cmd_options_load(CmdOptions *options, const int argc, const char **argv) {
    // 1. Build CLI Parser tied to 'options'
    FlagParser *parser = cmd_options_build_cli_parser(options);
    if (!parser) return INTERNAL_ERROR;

    // 2. Parse CLI (overwrites defaults in 'options', marks flags visited)
    const ParsingResult *res = flag_parser_parse(parser, argc, argv, false);

    if (options->help) {
        flag_parser_print_help(parser);
        flag_parser_free(parser);
        return HELP;
    }

    if (res->state != PARSE_SUCCESS) {
        flag_parser_free(parser);
        return res;
    }

    // 3. INI Parsing & Merging
    if (options->config_file) {
        // Create temp options with DEFAULTS (to have a clean base for INI)
        CmdOptions *ini_opt = cmd_options_create_defaults();

        // Parse INI into temp struct
        cmd_options_parse_ini_file(ini_opt, options->config_file);

        // Merge INI -> Options (respecting CLI priority via parser state)
        merge_ini_into_options(options, ini_opt, parser);

        // Cleanup temp
        cmd_options_destroy(ini_opt);
    }

    // 4. Validation
    const ParsingResult *val_res = validate_options(options);

    flag_parser_free(parser);
    return val_res;
}

/**
 * @brief Prints the current configuration to the log if verbosity allows.
 */
void print_configuration(const CmdOptions *options) {
    if_verbose(VERBOSE_DEBUG,
               "--- Options ---\n\n"
               "Mode:                %s\n"
               "Input file:          %s\n"
               "Config file:         %s\n"
               "Verbosity:           %u\n"
               "Plot path:           %s\n"
               "Nodes:               %u\n"
               "Seed:                %d\n"
               "Area:                %d,%d (side %u)\n"
               "\n\n"
               "--- Algorithms ---\n"
               "Nearest Neighbor:    %s\n"
               "  plot:              %s\n"
               "  cost:              %s\n"
               "  time limit:        %.3f\n"
               "\n"
               "VNS:                 %s\n"
               "  plot:              %s\n"
               "  cost:              %s\n"
               "  kicks MIN-MAX:     %u-%u\n"
               "  kicks reps:        %u\n"
               "  max stagnation:    %u\n"
               "  time limit:        %.3f\n"
               "\n"
               "Tabu Search:         %s\n"
               "  plot:              %s\n"
               "  cost:              %s\n"
               "  tenure, MIN-MAX:   %u-%u\n"
               "  max stagnation:    %u\n"
               "  time limit:        %.3f\n"
               "\n"
               "GRASP:               %s\n"
               "  plot:              %s\n"
               "  cost:              %s\n"
               "  p1:                %.3f\n"
               "  p2:                %.3f\n"
               "  time limit:        %.3f\n"
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

               options->nn_params.enable ? "ENABLED" : "DISABLED",
               options->nn_params.plot_file ? options->nn_params.plot_file : "(none)",
               options->nn_params.cost_file ? options->nn_params.cost_file : "(none)",
               options->nn_params.time_limit,

               options->vns_params.enable ? "ENABLED" : "DISABLED",
               options->vns_params.plot_file ? options->vns_params.plot_file : "(none)",
               options->vns_params.cost_file ? options->vns_params.cost_file : "(none)",
               options->vns_params.min_k,
               options->vns_params.max_k,
               options->vns_params.kick_repetitions,
               options->vns_params.max_stagnation,
               options->vns_params.time_limit,

               options->tabu_params.enable ? "ENABLED" : "DISABLED",
               options->tabu_params.plot_file ? options->tabu_params.plot_file : "(none)",
               options->tabu_params.cost_file ? options->tabu_params.cost_file : "(none)",
               options->tabu_params.min_tenure,
               options->tabu_params.max_tenure,
               options->tabu_params.max_stagnation,
               options->tabu_params.time_limit,

               options->grasp_params.enable ? "ENABLED" : "DISABLED",
               options->grasp_params.plot_file ? options->grasp_params.plot_file : "(none)",
               options->grasp_params.cost_file ? options->grasp_params.cost_file : "(none)",
               options->grasp_params.p1,
               options->grasp_params.p2,
               options->grasp_params.time_limit
    );
}
