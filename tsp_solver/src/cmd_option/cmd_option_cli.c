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
    flag_parser_add_ufloat(p, "--seconds", "-t", "Time limit", &opt->tsp.time_limit, FLAG_OPTIONAL);

    flag_parser_add_int(p, "--x-square", NULL, "Generation area X", &opt->tsp.generation_area.x_square, FLAG_OPTIONAL);
    flag_parser_add_int(p, "--y-square", NULL, "Generation area Y", &opt->tsp.generation_area.y_square, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--square-side", NULL, "Generation area side", &opt->tsp.generation_area.square_side,
                         FLAG_OPTIONAL);

    // NN
    flag_parser_add_bool(p, "--nn", NULL, "Enable NN", &opt->nn_params.enable, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--nn-plot", NULL, "NN plot file", &opt->nn_params.plot_file, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--nn-cost", NULL, "NN cost file", &opt->nn_params.cost_file, FLAG_OPTIONAL);

    // VNS
    flag_parser_add_bool(p, "--vns", NULL, "Enable VNS", &opt->vns_params.enable, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--vns-k", NULL, "VNS Kicks", &opt->vns_params.kick_repetitions, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--vns-n", NULL, "VNS N-Opt", &opt->vns_params.n_opt, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--vns-plot", NULL, "VNS plot file", &opt->vns_params.plot_file, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--vns-cost", NULL, "VNS cost file", &opt->vns_params.cost_file, FLAG_OPTIONAL);

    // Tabu
    flag_parser_add_bool(p, "--ts", NULL, "Enable Tabu", &opt->tabu_params.enable, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--ts-tenure", NULL, "TS Tenure", &opt->tabu_params.tenure, FLAG_OPTIONAL);
    flag_parser_add_uint(p, "--ts-stagnation", NULL, "TS Max Stagnation", &opt->tabu_params.max_stagnation,
                         FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--ts-plot", NULL, "TS plot file", &opt->tabu_params.plot_file, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--ts-cost", NULL, "TS cost file", &opt->tabu_params.cost_file, FLAG_OPTIONAL);

    // GRASP
    flag_parser_add_bool(p, "--grasp", NULL, "Enable GRASP", &opt->grasp_params.enable, FLAG_OPTIONAL);
    flag_parser_add_float(p, "--grasp-p1", NULL, "GRASP P1", &opt->grasp_params.p1, FLAG_OPTIONAL);
    flag_parser_add_float(p, "--grasp-p2", NULL, "GRASP P2", &opt->grasp_params.p2, FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--grasp-plot", NULL, "GRASP plot file", &opt->grasp_params.plot_file,
                                 FLAG_OPTIONAL);
    flag_parser_add_string_owned(p, "--grasp-cost", NULL, "GRASP cost file", &opt->grasp_params.cost_file,
                                 FLAG_OPTIONAL);

    return p;
}