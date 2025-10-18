#ifndef TSP_PARAMS_H
#define TSP_PARAMS_H
#include <flag.h>
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

typedef CmdOptions CmdOptions;

/**
 * @brief Structure holding command line options for TSP instance configuration.
 */
struct CmdOptions
{
    const char* config_file;
    unsigned int number_of_nodes; /**< Number of nodes to generate. */
    int seed;                     /**< Seed for random number generation. */
    Rectangle generation_area;    /**< Area parameters for node generation. */
    bool help;                    /**< Flag to display help message. */
    bool variable_neighborhood_search;
    bool nearest_neighbor;
    bool tabu_search;
    bool grasp;
    unsigned int kick_repetitions;
    unsigned int n_opt;
    unsigned int tenure;
    unsigned int max_stagnation;
    unsigned int timer_limit;
    unsigned int time_limit;      /**< Time limit for algorithm execution. */
    float p1;
    float p2;
    float p3;
};

/**
 * @brief Initializes command line options with default values.
 *
 * @return Pointer to a new CmdOptions instance.
 */
CmdOptions* init_cmd_options();

struct FlagsArray* init_flags_array();

void load_config(CmdOptions *cmd_options, const char **arg);
void free_cmd_option(CmdOptions *cmd_options);
#endif //TSP_PARAMS_H
