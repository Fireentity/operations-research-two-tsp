#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


#define SECTION_MATCH(s) !strcmp(section, s)
#define NAME_MATCH(n) !strcmp(name, n)

/**
 * @brief INI file parsing handler.
 * (Updated to use the nested structs)
 */
static int handler(void* user, const char* section, const char* name,
                   const char* value) {
    const IniUserData* data = (IniUserData*)user;
    CmdOptions* cmd_options = data->options;
    const FlagParser* parser = data->parser;

    const ParsingResult* result = SUCCESS;
    const char* flag_name_to_mark = NULL;

    if (SECTION_MATCH("tsp")) {
        if (NAME_MATCH("nodes")) {
            result = parse_uint(value, &cmd_options->tsp.number_of_nodes);
            flag_name_to_mark = "--nodes";
        }
        else if (NAME_MATCH("seed")) {
            result = parse_int(value, &cmd_options->tsp.seed);
            flag_name_to_mark = "--seed";
        }
        else if (NAME_MATCH("x-square")) {
            result = parse_int(value, &cmd_options->tsp.generation_area.x_square);
            flag_name_to_mark = "--x-square";
        }
        else if (NAME_MATCH("y-square")) {
            result = parse_int(value, &cmd_options->tsp.generation_area.y_square);
            flag_name_to_mark = "--y-square";
        }
        else if (NAME_MATCH("square-side")) {
            result = parse_uint(value, &cmd_options->tsp.generation_area.square_side);
            flag_name_to_mark = "--square-side";
        }
        else if (NAME_MATCH("seconds")) {
            result = parse_ufloat(value, &cmd_options->tsp.time_limit);
            flag_name_to_mark = "--seconds";
        }
        else if (NAME_MATCH("nearest-neighbor")) {
            cmd_options->nearest_neighbor = (strcmp(value, "true") == 0);
            flag_name_to_mark = "--nearest-neighbor";
        }
        else return 0;
    }
    else if (SECTION_MATCH("vns")) {
        if (NAME_MATCH("enabled")) {
            cmd_options->variable_neighborhood_search = (strcmp(value, "true") == 0);
            flag_name_to_mark = "--vns";
        }
        else if (NAME_MATCH("kick-repetitions")) {
            result = parse_uint(value, &cmd_options->vns_params.kick_repetitions);
            flag_name_to_mark = "--kick-repetitions";
        }
        else if (NAME_MATCH("n-opt")) {
            result = parse_uint(value, &cmd_options->vns_params.n_opt);
            flag_name_to_mark = "--n-opt";
        }
        else return 0;
    }
    else if (SECTION_MATCH("tabu")) {
        if (NAME_MATCH("enabled")) {
            cmd_options->tabu_search = (strcmp(value, "true") == 0);
            flag_name_to_mark = "--tabu-search";
        }
        else if (NAME_MATCH("tenure")) {
            result = parse_uint(value, &cmd_options->tabu_params.tenure);
            flag_name_to_mark = "--tenure";
        }
        else if (NAME_MATCH("max-stagnation")) {
            result = parse_uint(value, &cmd_options->tabu_params.max_stagnation);
            flag_name_to_mark = "--max-stagnation";
        }
        else return 0;
    }
    else if (SECTION_MATCH("grasp")) {
        if (NAME_MATCH("enabled")) {
            cmd_options->grasp = (strcmp(value, "true") == 0);
            flag_name_to_mark = "--grasp";
        }
        else if (NAME_MATCH("p1")) {
            result = parse_float(value, &cmd_options->grasp_params.p1);
            flag_name_to_mark = "--p1";
        }
        else if (NAME_MATCH("p2")) {
            result = parse_float(value, &cmd_options->grasp_params.p2);
            flag_name_to_mark = "--p2";
        }
        else return 0;
    }
    else return 0; // Unknown section

    // If parsing was successful, mark the flag as "visited"
    if (result->state == PARSE_SUCCESS) {
        // && flag_name_to_mark (implicit)
        flag_parser_mark_visited(parser, flag_name_to_mark);
    }

    return result->state == PARSE_SUCCESS;
}


/**
 * @brief Main function to parse all application options.
 * (This function remains logically unchanged)
 */
const ParsingResult* parse_application_options(CmdOptions* options, const int argc, const char** argv) {
    FlagParser* parser = create_app_parser(options);
    if (!parser) {
        return INTERNAL_ERROR;
    }

    // Parse command-line arguments (argv)
    const ParsingResult* result = flag_parser_parse(parser, argc, argv, false);

    // Check for --help
    if (options->help) {
        flag_parser_print_help(parser);
        flag_parser_free(parser);
        // Return a special result to tell main() to exit successfully
        return HELP;
    }

    // Check for parsing errors (e.g., unknown arg)
    if (result->state != PARSE_SUCCESS) {
        flag_parser_free(parser);
        return result;
    }

    // Load config file, if specified
    if (options->config_file) {
        IniUserData ini_data = {.options = options, .parser = parser};
        if (ini_parse(options->config_file, handler, &ini_data) < 0) {
            // Non-fatal error: just warn the user.
            printf("Warning: Could not open or parse config file: '%s'\n", options->config_file);
        }
    }

    // Final validation:
    // Checks that all MANDATORY flags were visited (.ini and command line)
    result = flag_parser_validate(parser);

    flag_parser_free(parser);
    return result;
}


