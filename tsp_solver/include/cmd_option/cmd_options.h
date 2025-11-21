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
    float time_limit;
} TspInstanceOptions;

typedef struct {
    bool enable;
    char *plot_file;
    char *cost_file;
} NNOptions;

typedef struct {
    bool enable;
    char *plot_file;
    char *cost_file;
    unsigned int kick_repetitions;
    unsigned int n_opt;
} VnsOptions;

typedef struct {
    bool enable;
    char *plot_file;
    char *cost_file;
    unsigned int tenure;
    unsigned int max_stagnation;
} TabuOptions;

typedef struct {
    bool enable;
    char *plot_file;
    char *cost_file;
    float p1;
    float p2;
} GraspOptions;

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
} CmdOptions;

CmdOptions *cmd_options_create_defaults(void);

void cmd_options_destroy(CmdOptions *options);

const ParsingResult *cmd_options_load(CmdOptions *options, int argc, const char **argv);

#endif // CMD_OPTIONS_H