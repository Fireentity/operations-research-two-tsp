#include "cmd_options.h"
#include "c_util.h"
#include <stdlib.h>
#include <string.h>

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

    return opt;
}

void cmd_options_destroy(CmdOptions *opt) {
    if (!opt) return;

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
}