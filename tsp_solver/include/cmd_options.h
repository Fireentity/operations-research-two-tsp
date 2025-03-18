#ifndef TSP_PARAMS_H
#define TSP_PARAMS_H
#include <parsing_result.h>
#include <stdbool.h>

/**
 * @brief Represents a rectangular area for node generation.
 */
typedef struct
{
    int x_square;         /**< X-coordinate of the generation area. */
    int y_square;         /**< Y-coordinate of the generation area. */
    unsigned int square_side; /**< Side length of the square. */
} Rectangle;

typedef struct CmdOptions CmdOptions;

/**
 * @brief Structure holding command line options for TSP instance configuration.
 */
struct CmdOptions
{
    unsigned int number_of_nodes; /**< Number of nodes to generate. */
    int seed;                     /**< Seed for random number generation. */
    Rectangle generation_area;    /**< Area parameters for node generation. */
    bool help;                    /**< Flag to display help message. */
    bool variable_neighborhood_search;
    bool nearest_neighbor;
    int kick_repetitions;
    unsigned int timer_limit;
    unsigned int time_limit;      /**< Time limit for algorithm execution. */
};

/**
 * @brief Initializes command line options with default values.
 *
 * @return Pointer to a new CmdOptions instance.
 */
struct CmdOptions* init_cmd_options();

/**
 * @brief Parses command line arguments into the provided command options.
 *
 * @param cmd_options Pointer to the CmdOptions structure.
 * @param argv Array of command line arguments.
 * @param argc Number of command line arguments.
 * @return ParsingResult indicating success or failure.
 */
ParsingResult parse_cli(CmdOptions* cmd_options, const char** argv);

#endif //TSP_PARAMS_H
