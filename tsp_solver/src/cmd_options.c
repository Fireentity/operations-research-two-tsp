#include <cmd_options.h>
#include <c_util.h>
#include <flag.h>
#include <flag_parser.h>
#include <parsing_result.h>
#include <parsing_util.h>

CmdOptions *init_cmd_options() {
    const CmdOptions cmd_options = {
        .generation_area = {
            .square_side = 0,
            .x_square = 0,
            .y_square = 0,
        },

        .help = false,
        .number_of_nodes = 0,
        .seed = 0,
        .time_limit = 0
    };
    return malloc_from_stack(&cmd_options, sizeof(cmd_options));
}

/**
 * @brief Sets the number of nodes in the command options.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_nodes(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->number_of_nodes);
}

/**
 * @brief Sets the seed value for random number generation.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_seed(CmdOptions *cmd_options, const char **arg) {
    return parse_int(*(arg + 1), &cmd_options->seed);
}

/**
 * @brief Sets the x-coordinate of the generation area.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_x_square(CmdOptions *cmd_options, const char **arg) {
    return parse_int(*(arg + 1), &cmd_options->generation_area.x_square);
}

/**
 * @brief Sets the y-coordinate of the generation area.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_y_square(CmdOptions *cmd_options, const char **arg) {
    return parse_int(*(arg + 1), &cmd_options->generation_area.y_square);
}

/**
 * @brief Sets the side length of the generation area.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_square_side(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->generation_area.square_side);
}

/**
 * @brief Sets the help flag in the command options.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_help(CmdOptions *cmd_options, const char **arg) {
    cmd_options->help = true;
    return PARSE_SUCCESS;
}

/**
 * @brief Enables the Variable Neighborhood Search (VNS) algorithm.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_vns(CmdOptions *cmd_options, const char **arg) {
    cmd_options->variable_neighborhood_search = true;
    return PARSE_SUCCESS;
}

/**
 * @brief Sets the number of kick repetitions for the VNS algorithm.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_kick_repetitions(CmdOptions *cmd_options, const char **arg) {
    return parse_int(*(arg + 1), &cmd_options->kick_repetitions);
}

/**
 * @brief Sets the time limit for the TSP algorithm.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_time_limit(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->time_limit);
}

/**
 * @brief Enables the Nearest Neighbor heuristic for the TSP algorithm.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_nearest_neighbor(CmdOptions *cmd_options, const char **arg) {
    cmd_options->nearest_neighbor = true;
    return PARSE_SUCCESS;
}

#define EMPTY_FLAGSARRAY (struct FlagsArray){NULL,0}

ParsingResult parse_cli(CmdOptions *cmd_options, const char **const argv) {
    const Flag *vns_children[] = {
        init_flag("--kick-repetitions", 1, set_kick_repetitions, true, EMPTY_FLAGSARRAY),
    };

    const Flag *tsp_flags[] = {
        init_flag("--nodes", 1, set_nodes, true, EMPTY_FLAGSARRAY),
        init_flag("--seed", 1, set_seed, false, EMPTY_FLAGSARRAY),
        init_flag("--x-square", 1, set_x_square, true, EMPTY_FLAGSARRAY),
        init_flag("--y-square", 1, set_y_square, true, EMPTY_FLAGSARRAY),
        init_flag("--square-side", 1, set_square_side, true, EMPTY_FLAGSARRAY),
        init_flag("--seconds", 1, set_time_limit, false, EMPTY_FLAGSARRAY),
        init_flag("--help", 0, set_help, false, EMPTY_FLAGSARRAY),
        init_flag("--vns", 0, set_vns, false, (struct FlagsArray){vns_children, 1}),
        init_flag("--nearest-neighbor", 0, set_nearest_neighbor, false, EMPTY_FLAGSARRAY)
    };
    FlagParser *parser = init_flag_parser((struct FlagsArray){tsp_flags, 9});
    const ParsingResult result = parse_flags_with_parser(cmd_options, parser, argv);
    free_flag_parser(parser);
    // TODO free delle flag

    return result;
}
