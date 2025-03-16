#ifndef TSP_PARAMS_H
#define TSP_PARAMS_H
#include <parsing_result.h>
#include <parsing_util.h>
#include <stdbool.h>
#include <tsp_instance.h>

/**
 * @brief Represents a rectangular area for node generation.
 */
typedef struct
{
    int x_square;         /**< X-coordinate of the generation area. */
    int y_square;         /**< Y-coordinate of the generation area. */
    unsigned int square_side; /**< Side length of the square. */
} Rectangle;

/**
 * @brief Structure holding command line options for TSP instance configuration.
 */
struct CmdOptions
{
    unsigned int number_of_nodes; /**< Number of nodes to generate. */
    int seed;                     /**< Seed for random number generation. */
    Rectangle generation_area;    /**< Area parameters for node generation. */
    bool help;                    /**< Flag to display help message. */
    unsigned int time_limit;      /**< Time limit for algorithm execution. */
};

/**
 * @brief Initializes command line options with default values.
 *
 * @return Pointer to a new CmdOptions instance.
 */
CmdOptions* init_cmd_options();

/**
 * @brief Parses command line arguments into the provided command options.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param argv Array of command line arguments.
 * @param argc Number of command line arguments.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult parse_cli(CmdOptions* cmd_options, const char** argv, int argc);

/**
 * @brief Sets the number of nodes in the command options.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_nodes(CmdOptions* cmd_options, const char** arg);

/**
 * @brief Sets the seed value for random number generation.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_seed(CmdOptions* cmd_options, const char** arg);

/**
 * @brief Sets the x-coordinate of the generation area.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_x_square(CmdOptions* cmd_options, const char** arg);

/**
 * @brief Sets the y-coordinate of the generation area.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_y_square(CmdOptions* cmd_options, const char** arg);

/**
 * @brief Sets the side length of the generation area.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_square_side(CmdOptions* cmd_options, const char** arg);

/**
 * @brief Sets the help flag in the command options.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_help(CmdOptions* cmd_options, const char** arg);

/**
 * @brief Sets the time limit for the TSP algorithm.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param arg Array of argument strings.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult set_time_limit(CmdOptions* cmd_options, const char** arg);

#endif //TSP_PARAMS_H
