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


CmdOptions* init_cmd_options() {
    // calloc zero-initializes everything, including sub-structs
    return calloc(1, sizeof(CmdOptions));
}

void free_cmd_option(CmdOptions* cmd_options) {
    if (!cmd_options) return;
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

    // --- Special Flags ---
    flag_parser_add_string(parser, "--config", "-c", "Path to .ini config file.",
                           &options->config_file, FLAG_OPTIONAL);

    flag_parser_add_bool(parser, "--help", "-h", "Show this help message.",
                         &options->help, FLAG_OPTIONAL);

#ifndef DISABLE_VERBOSE
    flag_parser_add_uint(parser, "--verbose", "-v",
                         "Enable verbose mode: 0 - None, 1 - Information, >=2 - Debug",
                         &options->verbosity, FLAG_OPTIONAL);
#endif

    // --- [tsp] section flags (now in options->tsp) ---
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

    // --- Algorithm Flags (Master switches) ---
    flag_parser_add_bool(parser, "--nearest-neighbor", NULL, "Use Nearest Neighbor heuristic.",
                         &options->nearest_neighbor, FLAG_OPTIONAL);

    flag_parser_add_bool(parser, "--vns", NULL, "Enable Variable Neighborhood Search.",
                         &options->variable_neighborhood_search, FLAG_OPTIONAL);

    flag_parser_add_bool(parser, "--tabu-search", NULL, "Enable Tabu Search.",
                         &options->tabu_search, FLAG_OPTIONAL);

    flag_parser_add_bool(parser, "--grasp", NULL, "Enable GRASP.",
                         &options->grasp, FLAG_OPTIONAL);


    // --- [vns] section flags (now in options->vns_params) ---
    flag_parser_add_uint(parser, "--kick-repetitions", NULL, "VNS kick repetitions.",
                         &options->vns_params.kick_repetitions, FLAG_OPTIONAL);

    flag_parser_add_uint(parser, "--n-opt", NULL, "VNS n-opt level (e.g., 2 for 2-opt).",
                         &options->vns_params.n_opt, FLAG_OPTIONAL);

    // --- [tabu] section flags (now in options->tabu_params) ---
    flag_parser_add_uint(parser, "--tenure", NULL, "Tabu tenure.",
                         &options->tabu_params.tenure, FLAG_OPTIONAL);

    flag_parser_add_uint(parser, "--max-stagnation", NULL, "Tabu max stagnation.",
                         &options->tabu_params.max_stagnation, FLAG_OPTIONAL);

    // --- [grasp] section flags (now in options->grasp_params) ---
    flag_parser_add_float(parser, "--p1", NULL, "GRASP p1 parameter.",
                          &options->grasp_params.p1, FLAG_OPTIONAL);

    flag_parser_add_float(parser, "--p2", NULL, "GRASP p2 parameter.",
                          &options->grasp_params.p2, FLAG_OPTIONAL);

    return parser;
}

// INI Handler

#define SECTION_MATCH(s) !strcmp(section, s)
#define NAME_MATCH(n) !strcmp(name, n)

typedef enum {
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
    // [tsp]
    {"tsp", "nodes", TYPE_UINT, offsetof(CmdOptions, tsp.number_of_nodes), "--nodes"},
    {"tsp", "seed", TYPE_INT, offsetof(CmdOptions, tsp.seed), "--seed"},
    {"tsp", "x-square", TYPE_INT, offsetof(CmdOptions, tsp.generation_area.x_square), "--x-square"},
    {"tsp", "y-square", TYPE_INT, offsetof(CmdOptions, tsp.generation_area.y_square), "--y-square"},
    {"tsp", "square-side", TYPE_UINT, offsetof(CmdOptions, tsp.generation_area.square_side), "--square-side"},
    {"tsp", "seconds", TYPE_UFLOAT, offsetof(CmdOptions, tsp.time_limit), "--seconds"},

    // [nn]
    {"nn", "enabled", TYPE_BOOL, offsetof(CmdOptions, nearest_neighbor), "--nearest-neighbor"},

    // [vns]
    {"vns", "enabled", TYPE_BOOL, offsetof(CmdOptions, variable_neighborhood_search), "--vns"},
    {"vns", "kick-repetitions", TYPE_UINT, offsetof(CmdOptions, vns_params.kick_repetitions), "--kick-repetitions"},
    {"vns", "n-opt", TYPE_UINT, offsetof(CmdOptions, vns_params.n_opt), "--n-opt"},

    // [tabu]
    {"tabu", "enabled", TYPE_BOOL, offsetof(CmdOptions, tabu_search), "--tabu-search"},
    {"tabu", "tenure", TYPE_UINT, offsetof(CmdOptions, tabu_params.tenure), "--tenure"},
    {"tabu", "max-stagnation", TYPE_UINT, offsetof(CmdOptions, tabu_params.max_stagnation), "--max-stagnation"},

    // [grasp]
    {"grasp", "enabled", TYPE_BOOL, offsetof(CmdOptions, grasp), "--grasp"},
    {"grasp", "p1", TYPE_FLOAT,offsetof(CmdOptions, grasp_params.p1), "--p1"},
    {"grasp", "p2", TYPE_FLOAT,offsetof(CmdOptions, grasp_params.p2), "--p2"}
};

/**
 * @brief INI file parsing handler.
 */
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
