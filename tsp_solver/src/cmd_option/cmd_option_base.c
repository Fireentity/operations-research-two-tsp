#include "cmd_options.h"
#include "cmd_option_internal.h"
#include "c_util.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/* Registry defining all CLI/INI mappings.*/
static const OptionMeta options_registry[] = {
    // GENERAL
    {"--verbosity", "-v", "Verbosity level (0-3)", "general", "verbosity", OPT_UINT, offsetof(CmdOptions, verbosity)},
    {"--plot-path", "-p", "Output directory for plots", "general", "plots_path", OPT_STRING, offsetof(CmdOptions, plots_path)},

    // TSP INSTANCE
    {"--mode", "-m", "Input mode (0=Random, 1=File)", "tsp", "mode", OPT_TSP_MODE, offsetof(CmdOptions, tsp.mode)},
    {"--file", "-f", "Input .tsp file path", "tsp", "file", OPT_STRING, offsetof(CmdOptions, tsp.input_file)},
    {"--nodes", "-n", "Number of nodes (Random mode)", "tsp", "nodes", OPT_UINT, offsetof(CmdOptions, tsp.number_of_nodes)},
    {"--seed", "-s", "Random seed", "tsp", "seed", OPT_INT, offsetof(CmdOptions, tsp.seed)},
    {"--x-square", NULL, "Generation area X origin", "tsp", "x-square", OPT_INT, offsetof(CmdOptions, tsp.generation_area.x_square)},
    {"--y-square", NULL, "Generation area Y origin", "tsp", "y-square", OPT_INT, offsetof(CmdOptions, tsp.generation_area.y_square)},
    {"--square-side", NULL, "Generation area side length", "tsp", "square-side", OPT_UINT, offsetof(CmdOptions, tsp.generation_area.square_side)},

    // NEAREST NEIGHBOR
    {"--nn", NULL, "Enable Nearest Neighbor", "nn", "enabled", OPT_BOOL, offsetof(CmdOptions, nn_params.enable)},
    {"--nn-seconds", NULL, "Time limit for NN", "nn", "seconds", OPT_UDOUBLE, offsetof(CmdOptions, nn_params.time_limit)},
    {"--nn-plot", NULL, "NN plot filename", "nn", "plot_file", OPT_STRING, offsetof(CmdOptions, nn_params.plot_file)},
    {"--nn-cost", NULL, "NN cost filename", "nn", "cost_file", OPT_STRING, offsetof(CmdOptions, nn_params.cost_file)},

    // VNS
    {"--vns", NULL, "Enable Variable Neighborhood Search", "vns", "enabled", OPT_BOOL, offsetof(CmdOptions, vns_params.enable)},
    {"--vns-min-k", NULL, "VNS Min K", "vns", "min_k", OPT_UINT, offsetof(CmdOptions, vns_params.min_k)},
    {"--vns-max-k", NULL, "VNS Max K", "vns", "max_k", OPT_UINT, offsetof(CmdOptions, vns_params.max_k)},
    {"--vns-k-reps", NULL, "Kick repetitions", "vns", "kick-repetitions", OPT_UINT, offsetof(CmdOptions, vns_params.kick_repetitions)},
    {"--vns-stagnation", NULL, "Max Stagnation", "vns", "max-stagnation", OPT_UINT, offsetof(CmdOptions, vns_params.max_stagnation)},
    {"--vns-seconds", NULL, "Time limit for VNS", "vns", "seconds", OPT_UDOUBLE, offsetof(CmdOptions, vns_params.time_limit)},
    {"--vns-plot", NULL, "VNS plot filename", "vns", "plot_file", OPT_STRING, offsetof(CmdOptions, vns_params.plot_file)},
    {"--vns-cost", NULL, "VNS cost filename", "vns", "cost_file", OPT_STRING, offsetof(CmdOptions, vns_params.cost_file)},

    // TABU SEARCH
    {"--ts", NULL, "Enable Tabu Search", "tabu", "enabled", OPT_BOOL, offsetof(CmdOptions, tabu_params.enable)},
    {"--ts-min-tenure", NULL, "Min Tenure", "tabu", "min-tenure", OPT_UINT, offsetof(CmdOptions, tabu_params.min_tenure)},
    {"--ts-max-tenure", NULL, "Max Tenure", "tabu", "max-tenure", OPT_UINT, offsetof(CmdOptions, tabu_params.max_tenure)},
    {"--ts-stagnation", NULL, "Max Stagnation", "tabu", "max-stagnation", OPT_UINT, offsetof(CmdOptions, tabu_params.max_stagnation)},
    {"--ts-seconds", NULL, "Time limit for Tabu", "tabu", "seconds", OPT_UDOUBLE, offsetof(CmdOptions, tabu_params.time_limit)},
    {"--ts-plot", NULL, "Tabu plot filename", "tabu", "plot_file", OPT_STRING, offsetof(CmdOptions, tabu_params.plot_file)},
    {"--ts-cost", NULL, "Tabu cost filename", "tabu", "cost_file", OPT_STRING, offsetof(CmdOptions, tabu_params.cost_file)},

    // GRASP
    {"--grasp", NULL, "Enable GRASP", "grasp", "enabled", OPT_BOOL, offsetof(CmdOptions, grasp_params.enable)},
    {"--grasp-rcl-size", NULL, "RCL Size", "grasp", "rcl-size", OPT_UINT, offsetof(CmdOptions, grasp_params.rcl_size)},
    {"--grasp-probability", NULL, "RCL Probability", "grasp", "probability", OPT_UDOUBLE, offsetof(CmdOptions, grasp_params.probability)},
    {"--grasp-stagnation", NULL, "Max Stagnation", "grasp", "max-stagnation", OPT_UINT, offsetof(CmdOptions, grasp_params.max_stagnation)},
    {"--grasp-seconds", NULL, "Time limit for GRASP", "grasp", "seconds", OPT_UDOUBLE, offsetof(CmdOptions, grasp_params.time_limit)},
    {"--grasp-plot", NULL, "GRASP plot filename", "grasp", "plot_file", OPT_STRING, offsetof(CmdOptions, grasp_params.plot_file)},
    {"--grasp-cost", NULL, "GRASP cost filename", "grasp", "cost_file", OPT_STRING, offsetof(CmdOptions, grasp_params.cost_file)},
};

const OptionMeta* cmd_options_get_metadata(void) {
    return options_registry;
}

size_t cmd_options_get_metadata_count(void) {
    return sizeof(options_registry) / sizeof(options_registry[0]);
}

/* Default sets intentionally provide stable baseline configuration. */

static void set_tsp_defaults(TspInstanceOptions *opt) {
    opt->mode = TSP_INPUT_MODE_RANDOM;
    opt->number_of_nodes = 100;
    opt->seed = 0;
    opt->input_file = NULL;
    opt->generation_area.x_square = 0;
    opt->generation_area.y_square = 0;
    opt->generation_area.square_side = 1000;
}

static void set_nn_defaults(NNOptions *opt) {
    opt->enable = false;
    opt->plot_file = strdup("NN-plot.png");
    opt->cost_file = strdup("NN-costs.txt");
}

static void set_vns_defaults(VnsOptions *opt) {
    opt->enable = false;
    opt->min_k = 3;
    opt->max_k = 10;
    opt->kick_repetitions = 1;
    opt->plot_file = strdup("VNS-plot.png");
    opt->cost_file = strdup("VNS-costs.txt");
}

static void set_tabu_defaults(TabuOptions *opt) {
    opt->enable = false;
    opt->min_tenure = 5;
    opt->max_tenure = 30;
    opt->max_stagnation = 200;
    opt->plot_file = strdup("TS-plot.png");
    opt->cost_file = strdup("TS-costs.txt");
}

static void set_grasp_defaults(GraspOptions *opt) {
    opt->enable = false;
    opt->rcl_size = 10;
    opt->probability = 0.3;
    opt->max_stagnation = 200;
    opt->plot_file = strdup("GR-plot.png");
    opt->cost_file = strdup("GR-costs.txt");
}

CmdOptions *cmd_options_create_defaults(void) {
    CmdOptions *opt = calloc(1, sizeof(CmdOptions));
    check_alloc(opt);

    opt->verbosity = 1;
    opt->config_file = NULL;
    opt->plots_path = NULL;

    set_tsp_defaults(&opt->tsp);
    set_nn_defaults(&opt->nn_params);
    set_vns_defaults(&opt->vns_params);
    set_tabu_defaults(&opt->tabu_params);
    set_grasp_defaults(&opt->grasp_params);

    if_verbose(VERBOSE_DEBUG, "Options initialized with defaults\n");

    return opt;
}

void cmd_options_destroy(CmdOptions *opt) {
    if (!opt) return;

    /* Freeing all dynamically allocated strings avoids leaks on reloads. */
    free(opt->config_file);
    free(opt->plots_path);
    free(opt->tsp.input_file);

    free(opt->nn_params.plot_file);
    free(opt->nn_params.cost_file);

    free(opt->vns_params.plot_file);
    free(opt->vns_params.cost_file);

    free(opt->tabu_params.plot_file);
    free(opt->tabu_params.cost_file);

    free(opt->grasp_params.plot_file);
    free(opt->grasp_params.cost_file);

    free(opt);

    if_verbose(VERBOSE_DEBUG, "Options destroyed\n");
}
