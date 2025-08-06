#include <cmd_options.h>
#include <c_util.h>
#include <flag.h>
#include <parsing_result.h>
#include <parsing_util.h>

CmdOptions *init_cmd_options() {
    const CmdOptions cmd_options = {};
    return malloc_from_stack(&cmd_options, sizeof(cmd_options));
}


/**
 * @brief Sets the number of nodes in the command options.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_nodes(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->number_of_nodes);
}

/**
 * @brief Sets the seed value for random number generation.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_seed(CmdOptions *cmd_options, const char **arg) {
    return parse_int(*(arg + 1), &cmd_options->seed);
}

/**
 * @brief Sets the x-coordinate of the generation area.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_x_square(CmdOptions *cmd_options, const char **arg) {
    return parse_int(*(arg + 1), &cmd_options->generation_area.x_square);
}

/**
 * @brief Sets the y-coordinate of the generation area.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_y_square(CmdOptions *cmd_options, const char **arg) {
    return parse_int(*(arg + 1), &cmd_options->generation_area.y_square);
}

/**
 * @brief Sets the side length of the generation area.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_square_side(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->generation_area.square_side);
}

/**
 * @brief Sets the help flag in the command options.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_help(CmdOptions *cmd_options, const char **arg) {
    cmd_options->help = true;
    return SUCCESS;
}

/**
 * @brief Enables the Variable Neighborhood Search (VNS) algorithm.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_vns(CmdOptions *cmd_options, const char **arg) {
    cmd_options->variable_neighborhood_search = true;
    return SUCCESS;
}

/**
 * @brief Sets the number of kick repetitions for the VNS algorithm.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_kick_repetitions(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->kick_repetitions);
}

const ParsingResult* set_tenure(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->tenure);
}

const ParsingResult* set_max_stagnation(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->max_stagnation);
}

const ParsingResult* set_p1(CmdOptions *cmd_options, const char **arg) {
    return parse_float(*(arg + 1), &cmd_options->p1);
}

const ParsingResult* set_p2(CmdOptions *cmd_options, const char **arg) {
    return parse_float(*(arg + 1), &cmd_options->p2);
}

/**
 * @brief Sets the time limit for the TSP algorithm.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_time_limit(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->time_limit);
}

const ParsingResult* set_n_opt(CmdOptions *cmd_options, const char **arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->n_opt);
}

/**
 * @brief Enables the Nearest Neighbor heuristic for the TSP algorithm.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_nearest_neighbor(CmdOptions *cmd_options, const char **arg) {
    cmd_options->nearest_neighbor = true;
    return SUCCESS;
}

/**
 * @brief Enables the Nearest Neighbor heuristic for the TSP algorithm.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_tabu_search(CmdOptions *cmd_options, const char **arg) {
    cmd_options->tabu_search = true;
    return SUCCESS;
}

/**
 * @brief Enables the Nearest Neighbor heuristic for the TSP algorithm.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
const ParsingResult* set_grasp(CmdOptions *cmd_options, const char **arg) {
    cmd_options->grasp = true;
    return SUCCESS;
}

struct FlagsArray init_flags_array() {
    // Init VNS flags
    const Flag *vns_children_data[] = {
        init_flag("--kick-repetitions", 1, set_kick_repetitions, true),
        init_flag("--n-opt", 1, set_n_opt, true)
    };
    const Flag **vns_children = malloc_from_stack(vns_children_data, sizeof(const Flag *) * 2);

    // Init Tabu flags
    const Flag *tabu_children_data[] = {
        init_flag("--tenure", 1, set_tenure, true),
        init_flag("--max-stagnation", 1, set_max_stagnation, true)
    };
    const Flag **tabu_children = malloc_from_stack(tabu_children_data, sizeof(const Flag *) * 2);

    // Init Grasp flags
    const Flag *grasp_children_data[] = {
        init_flag("--p1", 1, set_p1, true),
        init_flag("--p2", 1, set_p2, true),
    };
    const Flag **grasp_children = malloc_from_stack(grasp_children_data, sizeof(const Flag *) * 2);

    // Initialization of TSP flags
    const size_t tsp_count = 11;
    const Flag *tsp_flags_data[] = {
        init_flag("--nodes", 1, set_nodes, true),
        init_flag("--seed", 1, set_seed, false),
        init_flag("--x-square", 1, set_x_square, true),
        init_flag("--y-square", 1, set_y_square, true),
        init_flag("--square-side", 1, set_square_side, true),
        init_flag("--seconds", 1, set_time_limit, false),
        init_flag("--help", 0, set_help, false),
        init_flag_with_children("--vns", 0, set_vns, false,
                                (struct FlagsArray){vns_children, 2}),
        init_flag("--nearest-neighbor", 0, set_nearest_neighbor, false),
        init_flag_with_children("--tabu-search", 0, set_tabu_search,false,
                                (struct FlagsArray){tabu_children, 2}),
        init_flag_with_children("--grasp", 0, set_grasp,false,
                        (struct FlagsArray){grasp_children, 2})
    };
    const Flag **tsp_flags = malloc_from_stack(tsp_flags_data, sizeof(const Flag *) * tsp_count);

    return (struct FlagsArray){tsp_flags, tsp_count};
}
