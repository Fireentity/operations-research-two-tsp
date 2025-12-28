#include "cmd_options.h"
#include "cmd_option_internal.h"
#include "c_util.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

static const OptionMeta options_registry[] = {
    // GENERAL
    {"--verbosity", "-v", "Verbosity level (0-3)", "general", "verbosity", OPT_UINT, offsetof(CmdOptions, verbosity)},
    {"--plot-path", "-p", "Output directory for plots", "general", "plots_path", OPT_STRING, offsetof(CmdOptions, plots_path)},

    // TSP INSTANCE
    {"--mode", "-m", "Input mode (0=Random, 1=File)", "tsp_inst", "mode", OPT_TSP_MODE, offsetof(CmdOptions, inst.mode)},
    {"--file", "-f", "Input .tsp file path", "tsp_inst", "file", OPT_STRING, offsetof(CmdOptions, inst.input_file)},
    {"--nodes", "-n", "Number of nodes (Random mode)", "tsp_inst", "nodes", OPT_UINT, offsetof(CmdOptions, inst.number_of_nodes)},
    {"--seed", "-s", "Random seed", "tsp_inst", "seed", OPT_INT, offsetof(CmdOptions, inst.seed)},
    {"--x-square", NULL, "Generation area X origin", "tsp_inst", "x-square", OPT_INT, offsetof(CmdOptions, inst.generation_area.x_square)},
    {"--y-square", NULL, "Generation area Y origin", "tsp_inst", "y-square", OPT_INT, offsetof(CmdOptions, inst.generation_area.y_square)},
    {"--square-side", NULL, "Generation area side length", "tsp_inst", "square-side", OPT_UINT, offsetof(CmdOptions, inst.generation_area.square_side)},

    // TSP SOLUTION
    {"--sol-load-file", "-slf", "Input .tspsol file path", "tsp_sol", "load", OPT_STRING, offsetof(CmdOptions, sol.load_file)},
    {"--sol-save-file", "-ssf", "Output .tspsol file path", "tsp_sol", "save", OPT_STRING, offsetof(CmdOptions, sol.save_file)},

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

    // EXTRA MILEAGE
    {"--em", NULL, "Enable EXTRA MILEAGE", "em", "enabled", OPT_BOOL, offsetof(CmdOptions, em_params.enable)},
    {"--em-seconds", NULL, "Time limit for EXTRA MILEAGE", "em", "seconds", OPT_UDOUBLE, offsetof(CmdOptions, em_params.time_limit)},
    {"--em-plot", NULL, "EXTRA MILEAGE plot filename", "em", "plot_file", OPT_STRING, offsetof(CmdOptions, em_params.plot_file)},
    {"--em-cost", NULL, "EXTRA MILEAGE cost filename", "em", "cost_file", OPT_STRING, offsetof(CmdOptions, em_params.cost_file)},

    // GENETIC ALGORITHM
    {"--ga", NULL, "Enable Genetic Algorithm", "genetic", "enabled", OPT_BOOL, offsetof(CmdOptions, genetic_params.enable)},
    {"--ga-pop-size", NULL, "Population size", "genetic", "pop_size", OPT_UINT, offsetof(CmdOptions, genetic_params.population_size)},
    {"--ga-elite", NULL, "Elite count", "genetic", "elite_count", OPT_UINT, offsetof(CmdOptions, genetic_params.elite_count)},
    {"--ga-mutation", NULL, "Mutation rate", "genetic", "mutation_rate", OPT_UDOUBLE, offsetof(CmdOptions, genetic_params.mutation_rate)},
    {"--ga-cut-min", NULL, "Crossover cut min ratio", "genetic", "cut_min", OPT_UINT, offsetof(CmdOptions, genetic_params.crossover_cut_min_ratio)},
    {"--ga-cut-max", NULL, "Crossover cut max ratio", "genetic", "cut_max", OPT_UINT, offsetof(CmdOptions, genetic_params.crossover_cut_max_ratio)},
    {"--ga-seconds", NULL, "Time limit for GA", "genetic", "seconds", OPT_UDOUBLE, offsetof(CmdOptions, genetic_params.time_limit)},
    {"--ga-plot", NULL, "GA plot filename", "genetic", "plot_file", OPT_STRING, offsetof(CmdOptions, genetic_params.plot_file)},
    {"--ga-cost", NULL, "GA cost filename", "genetic", "cost_file", OPT_STRING, offsetof(CmdOptions, genetic_params.cost_file)},
    {"--ga-tournament", NULL, "Tournament size", "genetic", "tournament_size", OPT_UINT, offsetof(CmdOptions, genetic_params.tournament_size)},
    {"--ga-grasp-rcl", NULL, "Init GRASP RCL size", "genetic", "grasp_rcl", OPT_UINT, offsetof(CmdOptions, genetic_params.init_grasp_rcl_size)},
    {"--ga-grasp-prob", NULL, "Init GRASP probability", "genetic", "grasp_prob", OPT_UDOUBLE, offsetof(CmdOptions, genetic_params.init_grasp_prob)},
    {"--ga-grasp-perc", NULL, "Init GRASP percentage", "genetic", "grasp_percent", OPT_UINT, offsetof(CmdOptions, genetic_params.init_grasp_percent)},

    // BENDERS DECOMPOSITION
    {"--benders", NULL, "Enable Benders Decomposition", "benders", "enabled", OPT_BOOL, offsetof(CmdOptions, benders_params.enable)},
    {"--benders-seconds", NULL, "Time limit for Benders", "benders", "seconds", OPT_UDOUBLE, offsetof(CmdOptions, benders_params.time_limit)},
    {"--benders-iter", NULL, "Max iterations for Benders", "benders", "max_iterations", OPT_UINT, offsetof(CmdOptions, benders_params.max_iterations)},
    {"--benders-plot", NULL, "Benders plot filename", "benders", "plot_file", OPT_STRING, offsetof(CmdOptions, benders_params.plot_file)},
    {"--benders-cost", NULL, "Benders cost filename", "benders", "cost_file", OPT_STRING, offsetof(CmdOptions, benders_params.cost_file)},

    // BRANCH AND CUT
    {"--bc", NULL, "Enable Branch and Cut", "bc", "enabled", OPT_BOOL, offsetof(CmdOptions, bc_params.enable)},
    {"--bc-seconds", NULL, "Time limit for Branch and Cut", "bc", "seconds", OPT_UDOUBLE, offsetof(CmdOptions, bc_params.time_limit)},
    {"--bc-threads", NULL, "Number of threads (0=auto)", "bc", "threads", OPT_UINT, offsetof(CmdOptions, bc_params.num_threads)},
    {"--bc-plot", NULL, "Branch and Cut plot filename", "bc", "plot_file", OPT_STRING, offsetof(CmdOptions, bc_params.plot_file)},
    {"--bc-cost", NULL, "Branch and Cut cost filename", "bc", "cost_file", OPT_STRING, offsetof(CmdOptions, bc_params.cost_file)},

    // HARD FIXING (MATHEURISTIC)
    {"--hf", NULL, "Enable Hard Fixing", "hf", "enabled", OPT_BOOL, offsetof(CmdOptions, hf_params.enable)},
    {"--hf-seconds", NULL, "Time limit for Hard Fixing", "hf", "seconds", OPT_UDOUBLE, offsetof(CmdOptions, hf_params.time_limit)},
    {"--hf-rate", NULL, "Fixing rate (0.0-1.0)", "hf", "rate", OPT_UDOUBLE, offsetof(CmdOptions, hf_params.fixing_rate)},
    {"--hf-ratio", NULL, "Heuristic time ratio (0.0-1.0)", "hf", "ratio", OPT_UDOUBLE, offsetof(CmdOptions, hf_params.heuristic_ratio)},
    {"--hf-heuristic", NULL, "Heuristic (nn, em, vns, tabu, grasp)", "hf", "heuristic", OPT_STRING, offsetof(CmdOptions, hf_params.heuristic_name)},
    {"--hf-plot", NULL, "HF plot filename", "hf", "plot_file", OPT_STRING, offsetof(CmdOptions, hf_params.plot_file)},
    {"--hf-cost", NULL, "HF cost filename", "hf", "cost_file", OPT_STRING, offsetof(CmdOptions, hf_params.cost_file)},

    // LOCAL BRANCHING (MATHEURISTIC)
    {"--lb", NULL, "Enable Local Branching", "lb", "enabled", OPT_BOOL, offsetof(CmdOptions, lb_params.enable)},
    {"--lb-seconds", NULL, "Time limit for LB", "lb", "seconds", OPT_UDOUBLE, offsetof(CmdOptions, lb_params.time_limit)},
    {"--lb-k", NULL, "Neighborhood size k", "lb", "k", OPT_INT, offsetof(CmdOptions, lb_params.k)},
    {"--lb-ratio", NULL, "Heuristic ratio", "lb", "ratio", OPT_UDOUBLE, offsetof(CmdOptions, lb_params.heuristic_ratio)},
    {"--lb-heuristic", NULL, "Heuristic", "lb", "heuristic", OPT_STRING, offsetof(CmdOptions, lb_params.heuristic_name)},
    {"--lb-plot", NULL, "LB plot", "lb", "plot_file", OPT_STRING, offsetof(CmdOptions, lb_params.plot_file)},
    {"--lb-cost", NULL, "LB cost", "lb", "cost_file", OPT_STRING, offsetof(CmdOptions, lb_params.cost_file)},
};

const OptionMeta* cmd_options_get_metadata(void) {
    return options_registry;
}

size_t cmd_options_get_metadata_count(void) {
    return sizeof(options_registry) / sizeof(options_registry[0]);
}

static void set_tsp_inst_defaults(TspInstanceOptions *opt) {
    opt->mode = TSP_INPUT_MODE_RANDOM;
    opt->number_of_nodes = 100;
    opt->seed = 0;
    opt->input_file = NULL;
    opt->generation_area.x_square = 0;
    opt->generation_area.y_square = 0;
    opt->generation_area.square_side = 1000;
}
static void set_tsp_sol_defaults(TspSolutionOptions *opt) {
    opt->load_file=NULL;
    opt->save_file=NULL;
}
static void set_nn_defaults(NNOptions *opt) {
    opt->enable = false;
    opt->plot_file = strdup("NN-plot.png");
    opt->cost_file = strdup("NN-costs.png");
}

static void set_vns_defaults(VnsOptions *opt) {
    opt->enable = false;
    opt->min_k = 3;
    opt->max_k = 10;
    opt->kick_repetitions = 1;
    opt->plot_file = strdup("VNS-plot.png");
    opt->cost_file = strdup("VNS-costs.png");
}

static void set_tabu_defaults(TabuOptions *opt) {
    opt->enable = false;
    opt->min_tenure = 5;
    opt->max_tenure = 30;
    opt->max_stagnation = 200;
    opt->plot_file = strdup("TS-plot.png");
    opt->cost_file = strdup("TS-costs.png");
}

static void set_grasp_defaults(GraspOptions *opt) {
    opt->enable = false;
    opt->rcl_size = 10;
    opt->probability = 0.3;
    opt->max_stagnation = 200;
    opt->plot_file = strdup("GR-plot.png");
    opt->cost_file = strdup("GR-costs.png");
}

static void set_em_defaults(EMOptions *opt) {
    opt->enable = false;
    opt->plot_file = strdup("EM-plot.png");
    opt->cost_file = strdup("EM-costs.png");
}

static void set_genetic_defaults(GeneticOptions *opt) {
    opt->enable = false;
    opt->population_size = 1000;
    opt->elite_count = 1;
    opt->mutation_rate = 0.1;
    opt->crossover_cut_min_ratio = 25;
    opt->crossover_cut_max_ratio = 75;
    opt->plot_file = strdup("GA-plot.png");
    opt->cost_file = strdup("GA-costs.png");
    opt->tournament_size = 5;
    opt->init_grasp_rcl_size = 5;
    opt->init_grasp_prob = 0.2;
    opt->init_grasp_percent = 90;
}

static void set_benders_defaults(BendersOptions *opt) {
    opt->enable = false;
    opt->max_iterations = 200;
    opt->plot_file = strdup("Benders-plot.png");
    opt->cost_file = strdup("Benders-costs.png");
}

static void set_bc_defaults(BranchCutOptions *opt) {
    opt->enable = false;
    opt->time_limit = 60.0;
    opt->num_threads = 0;
    opt->plot_file = strdup("BC-plot.png");
    opt->cost_file = strdup("BC-costs.png");
}

static void set_hf_defaults(HardFixingOptions *opt) {
    opt->enable = false;
    opt->fixing_rate = 0.9;
    opt->heuristic_ratio = 0.2;
    opt->time_limit = 60.0;
    opt->plot_file = strdup("HF-plot.png");
    opt->cost_file = strdup("HF-costs.png");
    opt->heuristic_name = strdup("vns");
}

static void set_lb_defaults(LocalBranchingOptions *opt) {
    opt->enable = false;
    opt->k = 20;
    opt->time_limit = 60.0;
    opt->heuristic_ratio = 0.2;
    opt->plot_file = strdup("LB-plot.png");
    opt->cost_file = strdup("LB-costs.png");
    opt->heuristic_name = strdup("vns");
}

CmdOptions *cmd_options_create_defaults(void) {
    CmdOptions *opt = calloc(1, sizeof(CmdOptions));
    check_alloc(opt);

    opt->verbosity = 1;
    opt->config_file = NULL;
    opt->plots_path = NULL;

    set_tsp_inst_defaults(&opt->inst);
    set_tsp_sol_defaults(&opt->sol);
    set_nn_defaults(&opt->nn_params);
    set_vns_defaults(&opt->vns_params);
    set_tabu_defaults(&opt->tabu_params);
    set_grasp_defaults(&opt->grasp_params);
    set_em_defaults(&opt->em_params);
    set_genetic_defaults(&opt->genetic_params);
    set_benders_defaults(&opt->benders_params);
    set_bc_defaults(&opt->bc_params);
    set_hf_defaults(&opt->hf_params);
    set_lb_defaults(&opt->lb_params);

    if_verbose(VERBOSE_DEBUG, "Options initialized with defaults\n");

    return opt;
}

void cmd_options_destroy(CmdOptions *opt) {
    if (!opt) return;

    free(opt->config_file);
    free(opt->plots_path);
    free(opt->inst.input_file);

    free(opt->sol.load_file);
    free(opt->sol.save_file);

    free(opt->nn_params.plot_file);
    free(opt->nn_params.cost_file);

    free(opt->vns_params.plot_file);
    free(opt->vns_params.cost_file);

    free(opt->tabu_params.plot_file);
    free(opt->tabu_params.cost_file);

    free(opt->grasp_params.plot_file);
    free(opt->grasp_params.cost_file);

    free(opt->em_params.plot_file);
    free(opt->em_params.cost_file);

    free(opt->genetic_params.plot_file);
    free(opt->genetic_params.cost_file);

    free(opt->benders_params.plot_file);
    free(opt->benders_params.cost_file);

    free(opt->bc_params.plot_file);
    free(opt->bc_params.cost_file);

    free(opt->hf_params.plot_file);
    free(opt->hf_params.cost_file);
    free(opt->hf_params.heuristic_name);

    free(opt->lb_params.plot_file);
    free(opt->lb_params.cost_file);
    free(opt->lb_params.heuristic_name);

    free(opt);

    if_verbose(VERBOSE_DEBUG, "Options destroyed\n");
}