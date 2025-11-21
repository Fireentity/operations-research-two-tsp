#include "cmd_options.h"
#include "cmd_option_internal.h"
#include "flag_parser.h"
#include "c_util.h" // for memdup
#include <stdlib.h>
#include <string.h>

static const ParsingResult *validate_options(const CmdOptions *opt) {
    if (opt->tsp.mode == TSP_INPUT_MODE_RANDOM) {
        if (opt->tsp.number_of_nodes < 2) return MISSING_VALUE;
    } else if (opt->tsp.mode == TSP_INPUT_MODE_FILE) {
        if (!opt->tsp.input_file) return MISSING_MANDATORY_FLAG;
    }
    return SUCCESS;
}

typedef enum { OPT_INT, OPT_UINT, OPT_FLOAT, OPT_BOOL, OPT_STRING } OptType;

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
        case OPT_FLOAT:
            *(float *) dst = *(const float *) src;
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
        {"--seconds", OPT_FLOAT, &final->tsp.time_limit, &ini->tsp.time_limit},
        {"--file", OPT_STRING, &final->tsp.input_file, &ini->tsp.input_file},

        {"--x-square", OPT_INT, &final->tsp.generation_area.x_square, &ini->tsp.generation_area.x_square},
        {"--y-square", OPT_INT, &final->tsp.generation_area.y_square, &ini->tsp.generation_area.y_square},
        {"--square-side", OPT_UINT, &final->tsp.generation_area.square_side, &ini->tsp.generation_area.square_side},

        // NN
        {"--nn", OPT_BOOL, &final->nn_params.enable, &ini->nn_params.enable},
        {"--nn-plot", OPT_STRING, &final->nn_params.plot_file, &ini->nn_params.plot_file},
        {"--nn-cost", OPT_STRING, &final->nn_params.cost_file, &ini->nn_params.cost_file},

        // VNS
        {"--vns", OPT_BOOL, &final->vns_params.enable, &ini->vns_params.enable},
        {"--vns-k", OPT_UINT, &final->vns_params.kick_repetitions, &ini->vns_params.kick_repetitions},
        {"--vns-n", OPT_UINT, &final->vns_params.n_opt, &ini->vns_params.n_opt},
        {"--vns-plot", OPT_STRING, &final->vns_params.plot_file, &ini->vns_params.plot_file},
        {"--vns-cost", OPT_STRING, &final->vns_params.cost_file, &ini->vns_params.cost_file},

        // TABU
        {"--ts", OPT_BOOL, &final->tabu_params.enable, &ini->tabu_params.enable},
        {"--ts-tenure", OPT_UINT, &final->tabu_params.tenure, &ini->tabu_params.tenure},
        {"--ts-stagnation", OPT_UINT, &final->tabu_params.max_stagnation, &ini->tabu_params.max_stagnation},
        {"--ts-plot", OPT_STRING, &final->tabu_params.plot_file, &ini->tabu_params.plot_file},
        {"--ts-cost", OPT_STRING, &final->tabu_params.cost_file, &ini->tabu_params.cost_file},

        // GRASP
        {"--grasp", OPT_BOOL, &final->grasp_params.enable, &ini->grasp_params.enable},
        {"--grasp-p1", OPT_FLOAT, &final->grasp_params.p1, &ini->grasp_params.p1},
        {"--grasp-p2", OPT_FLOAT, &final->grasp_params.p2, &ini->grasp_params.p2},
        {"--grasp-plot", OPT_STRING, &final->grasp_params.plot_file, &ini->grasp_params.plot_file},
        {"--grasp-cost", OPT_STRING, &final->grasp_params.cost_file, &ini->grasp_params.cost_file},
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