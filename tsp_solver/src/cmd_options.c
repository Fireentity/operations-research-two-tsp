#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h> // For offsetof
#include <stdbool.h> // For bool in IniMapping
#include "cmd_options.h"
#include "flag_parser.h"
#include "ini.h"
#include <parsing_result.h>
#include <parsing_util.h>

#include "c_util.h"


CmdOptions* init_cmd_options() {
    CmdOptions* options = calloc(1, sizeof(CmdOptions));
    check_alloc(options);

    // NN Defaults
    options->nn_params.plot_file = strdup("NN-plot.png");
    check_alloc(options->nn_params.plot_file);
    options->nn_params.cost_file = strdup("NN-costs.txt");
    check_alloc(options->nn_params.cost_file);

    // VNS Defaults
    options->vns_params.plot_file = strdup("VNS-plot.png");
    check_alloc(options->vns_params.plot_file);
    options->vns_params.cost_file = strdup("VNS-costs.txt");
    check_alloc(options->vns_params.cost_file);

    // Tabu Search Defaults
    options->tabu_params.plot_file = strdup("TS-plot.png");
    check_alloc(options->tabu_params.plot_file);
    options->tabu_params.cost_file = strdup("TS-costs.txt");
    check_alloc(options->tabu_params.cost_file);

    // GRASP Defaults
    options->grasp_params.plot_file = strdup("GR-plot.png");
    check_alloc(options->grasp_params.plot_file);
    options->grasp_params.cost_file = strdup("GR-costs.txt");
    check_alloc(options->grasp_params.cost_file);

    return options;
}

void free_cmd_option(CmdOptions* cmd_options) {
    if (!cmd_options) return;

    free(cmd_options->config_file);
    free(cmd_options->plot_path);

    free(cmd_options->nn_params.plot_file);
    free(cmd_options->nn_params.cost_file);

    free(cmd_options->vns_params.plot_file);
    free(cmd_options->vns_params.cost_file);

    free(cmd_options->tabu_params.plot_file);
    free(cmd_options->tabu_params.cost_file);

    free(cmd_options->grasp_params.plot_file);
    free(cmd_options->grasp_params.cost_file);

    free(cmd_options);
}

// --- Struct for INI handler ---

/**
 * @brief Helper struct to pass both options and parser to the ini_handler.
 */
typedef struct {
    CmdOptions* options;
    FlagParser* parser;
} IniUserData;


/**
 * @brief Registers all command-line flags with the parser.
 * (Updated to use the nested structs)
 */
FlagParser* create_app_parser(CmdOptions* options) {
    FlagParser* parser = flag_parser_new(options);
    if (!parser) return NULL;

    // --- Special flags ---
    flag_parser_add_string_owned(parser, "--config", "-c", "Path to .ini config file.",
                                 &options->config_file, FLAG_OPTIONAL);

    flag_parser_add_bool(parser, "--help", "-h", "Show this help message.",
                         &options->help, FLAG_OPTIONAL);

#ifndef DISABLE_VERBOSE
    flag_parser_add_uint(parser, "--verbosity", "-v",
                         "Enable verbose mode: 0 - None, 1 - Information, >=2 - Debug",
                         &options->verbosity, FLAG_OPTIONAL);
#endif

    // --- [general] section flags ---
    flag_parser_add_string_owned(parser, "--plot-path", "-p", "Path to the plot folder for the output plots.",
                                 &options->plot_path, FLAG_OPTIONAL);

    // --- [tsp] section flags ---
    flag_parser_add_uint(parser, "--nodes", "-n", "Number of nodes.",
                         &options->tsp.number_of_nodes, FLAG_MANDATORY);
    flag_parser_add_int(parser, "--seed", "-s", "Random seed.",
                        &options->tsp.seed, FLAG_OPTIONAL);
    flag_parser_add_int(parser, "--x-square", NULL, "Generation area X coordinate.",
                        &options->tsp.generation_area.x_square, FLAG_MANDATORY);
    flag_parser_add_int(parser, "--y-square", NULL, "Generation area Y coordinate.",
                        &options->tsp.generation_area.y_square, FLAG_MANDATORY);
    flag_parser_add_uint(parser, "--square-side", NULL, "Generation area side length.",
                         &options->tsp.generation_area.square_side, FLAG_MANDATORY);
    flag_parser_add_ufloat(parser, "--seconds", "-t", "Time limit in seconds.",
                           &options->tsp.time_limit, FLAG_OPTIONAL);

    // --- [nn] section flags---
    flag_parser_add_bool(parser, "--nn", NULL, "Enable Nearest Neighbor heuristic.",
                         &options->nn_params.enable, FLAG_OPTIONAL);
    flag_parser_add_string_owned(parser, "--nn-plot", NULL, "NN plot filename.",
                                 &options->nn_params.plot_file, FLAG_OPTIONAL);
    flag_parser_add_string_owned(parser, "--nn-cost", NULL, "NN cost filename.",
                                 &options->nn_params.cost_file, FLAG_OPTIONAL);

    // --- [vns] section flags---
    flag_parser_add_bool(parser, "--vns", NULL, "Enable Variable Neighborhood Search.",
                         &options->vns_params.enable, FLAG_OPTIONAL);
    flag_parser_add_string_owned(parser, "--vns-plot", NULL, "VNS plot filename.",
                                 &options->vns_params.plot_file, FLAG_OPTIONAL);
    flag_parser_add_string_owned(parser, "--vns-cost", NULL, "VNS cost filename.",
                                 &options->vns_params.cost_file, FLAG_OPTIONAL);
    flag_parser_add_uint(parser, "--vns-k", NULL, "VNS kick repetitions.",
                         &options->vns_params.kick_repetitions, FLAG_OPTIONAL);
    flag_parser_add_uint(parser, "--vns-n", NULL, "VNS n-opt level (e.g., 2 for 2-opt).",
                         &options->vns_params.n_opt, FLAG_OPTIONAL);

    // --- [tabu] section flags ---
    flag_parser_add_bool(parser, "--ts", NULL, "Enable Tabu Search.",
                         &options->tabu_params.enable, FLAG_OPTIONAL);
    flag_parser_add_string_owned(parser, "--ts-plot", NULL, "TS plot filename.",
                                 &options->tabu_params.plot_file, FLAG_OPTIONAL);
    flag_parser_add_string_owned(parser, "--ts-cost", NULL, "TS cost filename.",
                                 &options->tabu_params.cost_file, FLAG_OPTIONAL);
    flag_parser_add_uint(parser, "--ts-tenure", NULL, "Tabu tenure.",
                         &options->tabu_params.tenure, FLAG_OPTIONAL);
    flag_parser_add_uint(parser, "--ts-stagnation", NULL, "Tabu max stagnation.",
                         &options->tabu_params.max_stagnation, FLAG_OPTIONAL);

    // --- [grasp] section flags ---
    flag_parser_add_bool(parser, "--grasp", NULL, "Enable GRASP.",
                         &options->grasp_params.enable, FLAG_OPTIONAL);
    flag_parser_add_string_owned(parser, "--grasp-plot", NULL, "GRASP plot filename.",
                                 &options->grasp_params.plot_file, FLAG_OPTIONAL);
    flag_parser_add_string_owned(parser, "--grasp-cost", NULL, "GRASP cost filename.",
                                 &options->grasp_params.cost_file, FLAG_OPTIONAL);
    flag_parser_add_float(parser, "--grasp-p1", NULL, "GRASP p1 parameter.",
                          &options->grasp_params.p1, FLAG_OPTIONAL);
    flag_parser_add_float(parser, "--grasp-p2", NULL, "GRASP p2 parameter.",
                          &options->grasp_params.p2, FLAG_OPTIONAL);

    return parser;
}

// INI Handler

#define SECTION_MATCH(s) !strcmp(section, s)
#define NAME_MATCH(n) !strcmp(name, n)

typedef enum {
    TYPE_STRING,
    TYPE_UINT,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_UFLOAT,
    TYPE_BOOL
} ValueType;

typedef struct {
    const char* section;
    const char* name;
    ValueType type;
    size_t offset;
    const char* flag_name;
} IniMapping;

static const IniMapping mappings[] = {
    // [general]
    {"general", "plot_path", TYPE_STRING, offsetof(CmdOptions, plot_path), "--plot-path"},
#ifndef DISABLE_VERBOSE
    {"general", "verbosity", TYPE_UINT, offsetof(CmdOptions, verbosity), "--verbosity"},
#endif
    // [tsp]
    {"tsp", "nodes", TYPE_UINT, offsetof(CmdOptions, tsp.number_of_nodes), "--nodes"},
    {"tsp", "seed", TYPE_INT, offsetof(CmdOptions, tsp.seed), "--seed"},
    {"tsp", "x-square", TYPE_INT, offsetof(CmdOptions, tsp.generation_area.x_square), "--x-square"},
    {"tsp", "y-square", TYPE_INT, offsetof(CmdOptions, tsp.generation_area.y_square), "--y-square"},
    {"tsp", "square-side", TYPE_UINT, offsetof(CmdOptions, tsp.generation_area.square_side), "--square-side"},
    {"tsp", "seconds", TYPE_UFLOAT, offsetof(CmdOptions, tsp.time_limit), "--seconds"},

    // [nn]
    {"nn", "enabled", TYPE_BOOL, offsetof(CmdOptions, nn_params.enable), "--nn"},
    {"nn", "plot_file", TYPE_STRING, offsetof(CmdOptions, nn_params.plot_file), "--nn-plot"},
    {"nn", "cost_file", TYPE_STRING, offsetof(CmdOptions, nn_params.cost_file), "--nn-cost"},

    // [vns]
    {"vns", "enabled", TYPE_BOOL, offsetof(CmdOptions, vns_params.enable), "--vns"},
    {"vns", "kick-repetitions", TYPE_UINT, offsetof(CmdOptions, vns_params.kick_repetitions), "--vns-k"},
    {"vns", "n-opt", TYPE_UINT, offsetof(CmdOptions, vns_params.n_opt), "--vns-n"},
    {"vns", "plot_file", TYPE_STRING, offsetof(CmdOptions, vns_params.plot_file), "--vns-plot"},
    {"vns", "cost_file", TYPE_STRING, offsetof(CmdOptions, vns_params.cost_file), "--vns-cost"},

    // [tabu]
    {"tabu", "enabled", TYPE_BOOL, offsetof(CmdOptions, tabu_params.enable), "--ts"},
    {"tabu", "tenure", TYPE_UINT, offsetof(CmdOptions, tabu_params.tenure), "--ts-tenure"},
    {"tabu", "max-stagnation", TYPE_UINT, offsetof(CmdOptions, tabu_params.max_stagnation), "--ts-stagnation"},
    {"tabu", "plot_file", TYPE_STRING, offsetof(CmdOptions, tabu_params.plot_file), "--ts-plot"},
    {"tabu", "cost_file", TYPE_STRING, offsetof(CmdOptions, tabu_params.cost_file), "--ts-cost"},

    // [grasp]
    {"grasp", "enabled", TYPE_BOOL, offsetof(CmdOptions, grasp_params.enable), "--grasp"},
    {"grasp", "p1", TYPE_FLOAT, offsetof(CmdOptions, grasp_params.p1), "--grasp-p1"},
    {"grasp", "p2", TYPE_FLOAT, offsetof(CmdOptions, grasp_params.p2), "--grasp-p2"},
    {"grasp", "plot_file", TYPE_STRING, offsetof(CmdOptions, grasp_params.plot_file), "--grasp-plot"},
    {"grasp", "cost_file", TYPE_STRING, offsetof(CmdOptions, grasp_params.cost_file), "--grasp-cost"}
};

static int handler(void* user, const char* section, const char* name,
                   const char* value) {
    const IniUserData* data = (IniUserData*)user;
    CmdOptions* cmd_options = data->options;
    const FlagParser* parser = data->parser;

    const ParsingResult* result = SUCCESS;
    const char* flag_name_to_mark = NULL;

    const int num_mappings = sizeof(mappings) / sizeof(mappings[0]);
    bool match_found = false;

    for (int i = 0; i < num_mappings; i++) {
        const IniMapping* m = &mappings[i];

        if (SECTION_MATCH(m->section) && NAME_MATCH(m->name)) {
            match_found = true;
            flag_name_to_mark = m->flag_name;

            if (flag_parser_is_visited(parser, m->flag_name)) {
                result = SUCCESS;
                break;
            }

            void* dest = (char*)cmd_options + m->offset;

            switch (m->type) {
            case TYPE_UINT:
                result = parse_uint(value, dest);
                break;
            case TYPE_INT:
                result = parse_int(value, dest);
                break;
            case TYPE_FLOAT:
                result = parse_float(value, dest);
                break;
            case TYPE_UFLOAT:
                result = parse_ufloat(value, dest);
                break;
            case TYPE_BOOL:
                *(bool*)dest = strcmp(value, "true") == 0;
                break;
            case TYPE_STRING:
                ;
                char** dest_ptr = dest;
                if (*dest_ptr != NULL) {
                    free(*dest_ptr);
                }
                result = parse_string(value, dest_ptr);
                break;
            }
            break;
        }
    }

    if (!match_found) {
        return 0; // Unknown section/name combination
    }

    if (result->state == PARSE_SUCCESS && flag_name_to_mark != NULL) {
        flag_parser_mark_visited(parser, flag_name_to_mark);
    }

    return result->state == PARSE_SUCCESS;
}


/**
 * @brief Main function to parse all application options.
 */
const ParsingResult* parse_application_options(CmdOptions* options, const int argc, const char** argv) {
    FlagParser* parser = create_app_parser(options);
    if (!parser) {
        return INTERNAL_ERROR;
    }

    const ParsingResult* result = flag_parser_parse(parser, argc, argv, false);

    if (options->help) {
        flag_parser_print_help(parser);
        flag_parser_free(parser);
        return HELP;
    }

    if (result->state != PARSE_SUCCESS) {
        flag_parser_free(parser);
        return result;
    }

    if (options->config_file) {
        IniUserData ini_data = {.options = options, .parser = parser};
        if (ini_parse(options->config_file, handler, &ini_data) < 0) {
            printf("Warning: Could not open or parse config file: '%s'\n", options->config_file);
        }
    }


    result = flag_parser_validate(parser);

    flag_parser_free(parser);
    return result;
}
