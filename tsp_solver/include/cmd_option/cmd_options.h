#ifndef CMD_OPTIONS_H
#define CMD_OPTIONS_H

#include <stdbool.h>
#include "parsing_result.h"

typedef enum {
    TSP_INPUT_MODE_RANDOM = 0,
    TSP_INPUT_MODE_FILE
} TspInputMode;

typedef struct {
    int x_square;
    int y_square;
    unsigned int square_side;
} GenerationArea;

typedef struct {
    TspInputMode mode;
    unsigned int number_of_nodes;
    int seed;
    char *input_file;
    GenerationArea generation_area;
} TspInstanceOptions;

typedef struct {
    bool enable;
    char *plot_file;
    char *cost_file;
    double time_limit;
} NNOptions;

typedef struct {
    bool enable;
    char *plot_file;
    char *cost_file;
    unsigned int min_k;
    unsigned int max_k;
    unsigned int kick_repetitions;
    double time_limit;
    unsigned int max_stagnation;
} VnsOptions;

typedef struct {
    bool enable;
    char *plot_file;
    char *cost_file;
    unsigned int min_tenure;
    unsigned int max_tenure;
    unsigned int max_stagnation;
    double time_limit;
} TabuOptions;

typedef struct {
    bool enable;
    char *plot_file;
    char *cost_file;
    unsigned int rcl_size;
    double probability;
    unsigned int max_stagnation;
    double time_limit;
} GraspOptions;

typedef struct {
    bool enable;
    char *plot_file;
    char *cost_file;
    double time_limit;
} EMOptions;

typedef struct {
    bool enable;
    char *plot_file;
    char *cost_file;
    double time_limit;
    unsigned int population_size;
    unsigned int elite_count;
    double mutation_rate;
    unsigned int crossover_cut_min_ratio;
    unsigned int crossover_cut_max_ratio;
    int tournament_size;
    int init_grasp_rcl_size;
    double init_grasp_prob;
    int init_grasp_percent;
} GeneticOptions;

typedef struct {
    char *config_file;
    char *plots_path;
    bool help;
    unsigned int verbosity;

    TspInstanceOptions tsp;
    NNOptions nn_params;
    VnsOptions vns_params;
    TabuOptions tabu_params;
    GraspOptions grasp_params;
    EMOptions em_params;
    GeneticOptions genetic_params;
} CmdOptions;

CmdOptions *cmd_options_create_defaults(void);

void cmd_options_destroy(CmdOptions *options);

const ParsingResult *cmd_options_load(CmdOptions *options, int argc, const char **argv);

/**
 * @brief Prints the current configuration to the log if verbosity allows.
 */
void print_configuration(const CmdOptions *options);

#endif // CMD_OPTIONS_H
