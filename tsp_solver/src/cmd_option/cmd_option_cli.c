#include "cmd_options.h"
#include "cmd_option_internal.h"
#include "flag_parser.h"
#include <stddef.h>

FlagParser *cmd_options_build_cli_parser(CmdOptions *opt) {
    FlagParser *p = flag_parser_new(opt);
    if (!p) return NULL;

    // General
    flag_parser_add_bool(p, "--help", "-h", "Show help", &opt->help, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--config", "-c", "Config INI file", &opt->config_file, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--plot-path", "-p", "Output directory", &opt->plots_path, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--verbosity", "-v", "Verbosity (0-3)", &opt->verbosity, FLAG_OPTIONAL);

    // TSP Instance
    flag_parser_add_int(p, "--mode", "-m", "Input mode (0=Random, 1=File)", (int *) &opt->tsp.mode, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--file", "-f", "Input file path (.tsp)", &opt->tsp.input_file, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--nodes", "-n", "Num nodes (Random)", &opt->tsp.number_of_nodes, FLAG_OPTIONAL);
    flag_parser_add_int(p, "--seed", "-s", "Seed", &opt->tsp.seed, FLAG_OPTIONAL);
    flag_parser_add_int(p, "--x-square", NULL, "Generation area X", &opt->tsp.generation_area.x_square, FLAG_OPTIONAL);
    flag_parser_add_int(p, "--y-square", NULL, "Generation area Y", &opt->tsp.generation_area.y_square, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--square-side", NULL, "Generation area side", &opt->tsp.generation_area.square_side,
                         FLAG_OPTIONAL);

    // NN
    flag_parser_add_bool(p, "--nn", NULL, "Enable NN", &opt->nn_params.enable, FLAG_OPTIONAL);
    flag_parser_add_double(p, "--nn-seconds", NULL, "Time limit for NN in seconds", &opt->nn_params.time_limit,
                           FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--nn-plot", NULL, "NN plot file", &opt->nn_params.plot_file, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--nn-cost", NULL, "NN cost file", &opt->nn_params.cost_file, FLAG_OPTIONAL);

    // VNS
    flag_parser_add_bool(p, "--vns", NULL, "Enable VNS", &opt->vns_params.enable, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--vns-min-k", NULL, "VNS Min K", &opt->vns_params.min_k, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--vns-max-k", NULL, "VNS Max K", &opt->vns_params.max_k, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--vns-k-reps", NULL, "VNS Kicks Repetitions", &opt->vns_params.kick_repetitions,
                         FLAG_OPTIONAL);
    flag_parser_add_udouble(p, "--vns-seconds", NULL, "Time limit for VNS in seconds", &opt->vns_params.time_limit,
                            FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--vns-plot", NULL, "VNS plot file", &opt->vns_params.plot_file, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--vns-cost", NULL, "VNS cost file", &opt->vns_params.cost_file, FLAG_OPTIONAL);

    // Tabu
    flag_parser_add_bool(p, "--ts", NULL, "Enable Tabu", &opt->tabu_params.enable, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--ts-min-tenure", NULL, "TS Min Tenure", &opt->tabu_params.min_tenure, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--ts-max-tenure", NULL, "TS Max Tenure", &opt->tabu_params.max_tenure, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--ts-stagnation", NULL, "TS Max Stagnation", &opt->tabu_params.max_stagnation,
                         FLAG_OPTIONAL);
    flag_parser_add_udouble(p, "--ts-seconds", NULL, "Time limit for TS in seconds", &opt->tabu_params.time_limit,
                            FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--ts-plot", NULL, "TS plot file", &opt->tabu_params.plot_file, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--ts-cost", NULL, "TS cost file", &opt->tabu_params.cost_file, FLAG_OPTIONAL);

    // GRASP
    flag_parser_add_bool(p, "--grasp", NULL, "Enable GRASP", &opt->grasp_params.enable, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--grasp-rcl-size", NULL, "GRASP RCL Size", &opt->grasp_params.rcl_size, FLAG_OPTIONAL);
    flag_parser_add_udouble(p, "--grasp-probability", NULL, "GRASP Probability", &opt->grasp_params.probability,
                            FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--grasp-stagnation", NULL, "GRASP Max Stagnation", &opt->grasp_params.max_stagnation,
                         FLAG_OPTIONAL);
    flag_parser_add_udouble(p, "--grasp-seconds", NULL, "Time limit for GRASP in seconds",
                            &opt->grasp_params.time_limit, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--grasp-plot", NULL, "GRASP plot file", &opt->grasp_params.plot_file,
                                 FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--grasp-cost", NULL, "GRASP cost file", &opt->grasp_params.cost_file,
                                 FLAG_OPTIONAL);

    return p;
}
