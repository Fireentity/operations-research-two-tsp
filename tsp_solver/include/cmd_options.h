#ifndef CMD_OPTIONS_H
#define CMD_OPTIONS_H

#include <stdbool.h>
#include "parsing_result.h"

/**
 * @brief Rectangular area for node generation.
 */
typedef struct
{
    int x_square;
    int y_square;
    unsigned int square_side;
} GenerationArea;

/**
 * @brief Parameters defining the TSP problem instance.
 */
typedef struct
{
    unsigned int number_of_nodes;
    int seed;
    GenerationArea generation_area;
    float time_limit;
} TspInstanceOptions;

/**
 * @brief Options for the Variable Neighborhood Search (VNS) algorithm.
 */
typedef struct
{
    unsigned int kick_repetitions;
    unsigned int n_opt;
} VnsOptions;

/**
 * @brief Options for the Tabu Search algorithm.
 */
typedef struct
{
    unsigned int tenure;
    unsigned int max_stagnation;
} TabuOptions;

/**
 * @brief Options for the GRASP algorithm.
 */
typedef struct
{
    float p1;
    float p2;
} GraspOptions;


/**
 * @brief Main structure holding all configuration options.
 */
typedef struct
{
    // --- Program Behavior ---
    const char* config_file; /**< Path to the .ini configuration file. */
    bool help;               /**< Flag to display the help message. */
#ifndef DISABLE_VERBOSE
    unsigned int verbosity;  /**< Flag to set verbosity level. */
#endif

    // --- TSP Instance Definition ---
    TspInstanceOptions tsp;  /**< Parameters for the problem instance. */

    // --- Algorithm Selection (Master Flags) ---
    bool nearest_neighbor;
    bool variable_neighborhood_search;
    bool tabu_search;
    bool grasp;

    // --- Algorithm-Specific Parameters ---
    VnsOptions vns_params;
    TabuOptions tabu_params;
    GraspOptions grasp_params;

} CmdOptions;

/**
 * @brief Initializes options with default (zero-initialized) values.
 * @return Pointer to a new CmdOptions instance.
 */
CmdOptions* init_cmd_options();

/**
 * @brief Frees the memory allocated for the CmdOptions instance.
 */
void free_cmd_option(CmdOptions* cmd_options);

/**
 * @brief Parses all application options (from argv and config file).
 *
 * @param options A pointer to the CmdOptions struct to be populated.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return A const ParsingResult* (e.g., SUCCESS, PARSE_UNKNOWN_ARG).
 */
const ParsingResult* parse_application_options(CmdOptions* options, int argc, const char** argv);


#endif // CMD_OPTIONS_H