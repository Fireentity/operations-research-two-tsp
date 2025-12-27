#include "cmd_options.h"
#include "cmd_option_internal.h"
#include "flag_parser.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>

// Validation rejects configurations that would lead to undefined solver behavior.
static const ParsingResult *validate_options(const CmdOptions *opt) {
    if_verbose(VERBOSE_DEBUG, "Starting configuration validation...\n");

    // TSP instance validation
    if (opt->inst.mode == TSP_INPUT_MODE_FILE) {
        if (!opt->inst.input_file || strlen(opt->inst.input_file) == 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] Mode FILE requires a valid --file.\n");
            return MISSING_MANDATORY_FLAG;
        }
    } else {
        if (opt->inst.number_of_nodes < 2) {
            if_verbose(VERBOSE_INFO, "[Config Error] Random mode requires at least 2 nodes.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->inst.generation_area.square_side == 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] Generation area side must be > 0.\n");
            return WRONG_VALUE_TYPE;
        }
    }

    // NN validation
    if (opt->nn_params.enable) {
        if (opt->nn_params.time_limit < 0.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] NN: time limit cannot be negative.\n");
            return WRONG_VALUE_TYPE;
        }
    }

    // VNS validation
    if (opt->vns_params.enable) {
        if (opt->vns_params.min_k < 2 || opt->vns_params.max_k < 2) {
            if_verbose(VERBOSE_INFO, "[Config Error] VNS: min_k and max_k must be >= 2.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->vns_params.min_k > opt->vns_params.max_k) {
            if_verbose(VERBOSE_INFO, "[Config Error] VNS: min_k cannot exceed max_k.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->vns_params.kick_repetitions == 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] VNS: kick repetitions must be > 0.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->vns_params.max_stagnation == 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] VNS: max stagnation must be > 0.\n");
            return WRONG_VALUE_TYPE;
        }
    }

    //Tabu validation
    if (opt->tabu_params.enable) {
        if (opt->tabu_params.min_tenure == 0 || opt->tabu_params.max_tenure == 0 || opt->tabu_params.min_tenure > opt->
            tabu_params.max_tenure) {
            if_verbose(VERBOSE_INFO, "[Config Error] Tabu: invalid tenure range.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->tabu_params.max_stagnation == 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] Tabu: max stagnation must be > 0.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->tabu_params.time_limit < 0.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] Tabu: time limit cannot be negative.\n");
            return WRONG_VALUE_TYPE;
        }
    }

    // GRASP validation
    if (opt->grasp_params.enable) {
        if (opt->grasp_params.rcl_size < 1) {
            if_verbose(VERBOSE_INFO, "[Config Error] GRASP: RCL size must be >= 1.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->grasp_params.probability < 0.0 || opt->grasp_params.probability > 1.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] GRASP: probability must be in [0,1].\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->grasp_params.max_stagnation <= 0) {
            if_verbose(VERBOSE_INFO, "[Config Error] GRASP: max stagnation must be > 0.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->grasp_params.time_limit < 0.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] GRASP: time limit cannot be negative.\n");
            return WRONG_VALUE_TYPE;
        }
    }

    // Extra Mileage validation
    if (opt->em_params.enable) {
        if (opt->em_params.time_limit < 0.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] EM: time limit cannot be negative.\n");
            return WRONG_VALUE_TYPE;
        }
    }

    // Genetic Algorithm validation
    if (opt->genetic_params.enable) {
        if (opt->genetic_params.population_size < 2) {
            if_verbose(VERBOSE_INFO, "[Config Error] Genetic: Population size must be at least 2.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->genetic_params.elite_count < 0 || opt->genetic_params.elite_count >= opt->genetic_params.population_size) {
            if_verbose(VERBOSE_INFO, "[Config Error] Genetic: Elite count must be >= 0 and < population size.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->genetic_params.mutation_rate < 0.0 || opt->genetic_params.mutation_rate > 1.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] Genetic: Mutation rate must be in [0, 1].\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->genetic_params.crossover_cut_min_ratio < 0 || opt->genetic_params.crossover_cut_min_ratio > 100) {
            if_verbose(VERBOSE_INFO, "[Config Error] Genetic: Crossover cut min ratio must be in [0, 100].\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->genetic_params.crossover_cut_max_ratio < 0 || opt->genetic_params.crossover_cut_max_ratio > 100) {
            if_verbose(VERBOSE_INFO, "[Config Error] Genetic: Crossover cut max ratio must be in [0, 100].\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->genetic_params.crossover_cut_min_ratio > opt->genetic_params.crossover_cut_max_ratio) {
            if_verbose(VERBOSE_INFO, "[Config Error] Genetic: Crossover min ratio cannot be greater than max ratio.\n");
            return WRONG_VALUE_TYPE;
        }
        if (opt->genetic_params.time_limit < 0.0) {
            if_verbose(VERBOSE_INFO, "[Config Error] Genetic: Time limit cannot be negative.\n");
            return WRONG_VALUE_TYPE;
        }
    }

    // Benders Decomposition validation
    if (opt->benders_params.enable) {
        if (opt->benders_params.time_limit < 0.0) {
             if_verbose(VERBOSE_INFO, "[Config Error] Benders: time limit cannot be negative.\n");
             return WRONG_VALUE_TYPE;
        }
        if (opt->benders_params.max_iterations == 0) {
             if_verbose(VERBOSE_INFO, "[Config Error] Benders: max iterations must be > 0.\n");
             return WRONG_VALUE_TYPE;
        }
    }

    // Branch & Cut validation
    if (opt->bc_params.enable) {
        if (opt->bc_params.time_limit < 0.0) {
             if_verbose(VERBOSE_INFO, "[Config Error] Branch & Cut: time limit cannot be negative.\n");
             return WRONG_VALUE_TYPE;
        }
    }

    /* Warn if no metaheuristic is active; execution will be short-lived. */
    if (!opt->nn_params.enable && !opt->vns_params.enable && !opt->tabu_params.enable && !opt->grasp_params.enable &&
        !opt->em_params.enable && !opt->genetic_params.enable && !opt->benders_params.enable && !opt->bc_params.enable) {
        if_verbose(VERBOSE_INFO, "[Warning] No algorithms enabled.\n");
    }

    if_verbose(VERBOSE_DEBUG, "Configuration validation completed successfully.\n");
    return SUCCESS;
}

/* merge_ini_into_options copies values where CLI has not overridden them. */
static void copy_option_value(OptionType type, void *dst, const void *src) {
    switch (type) {
        case OPT_INT:
        case OPT_TSP_MODE:
            *(int *) dst = *(const int *) src;
            break;

        case OPT_UINT:
            *(unsigned int *) dst = *(const unsigned int *) src;
            break;

        case OPT_DOUBLE:
        case OPT_UDOUBLE:
            *(double *) dst = *(const double *) src;
            break;

        case OPT_BOOL:
            *(bool *) dst = *(const bool *) src;
            break;

        case OPT_STRING:
            free(*(char **) dst);
            *(char **) dst = *(char * const *) src
                                 ? strdup(*(char * const *) src)
                                 : NULL;
            break;
    }
}

static void merge_ini_into_options(CmdOptions *final,
                                   const CmdOptions *ini,
                                   const FlagParser *cli) {
    const OptionMeta *meta = cmd_options_get_metadata();
    const size_t count = cmd_options_get_metadata_count();

    char *fb = (char *) final;
    const char *ib = (const char *) ini;

    for (size_t i = 0; i < count; i++) {
        const OptionMeta *m = &meta[i];

        /* CLI takes priority; INI applies only to non-overridden fields. */
        if (!flag_parser_is_visited(cli, m->cli_long)) {
            void *dst = fb + m->offset;
            const void *src = ib + m->offset;
            copy_option_value(m->type, dst, src);
        }
    }
}

const ParsingResult *cmd_options_load(CmdOptions *options,
                                      const int argc,
                                      const char **argv) {
    FlagParser *parser = cmd_options_build_cli_parser(options);
    if (!parser) return INTERNAL_ERROR;

    const ParsingResult *res = flag_parser_parse(parser, argc, argv, false);

    if (options->help) {
        flag_parser_print_help(parser);
        flag_parser_free(parser);
        return HELP;
    }

    if (res->state != PARSE_SUCCESS) {
        flag_parser_free(parser);
        return res;
    }

    /* INI file extends defaults and is overridden by CLI. */
    if (options->config_file) {
        if_verbose(VERBOSE_DEBUG, "Loading configuration file: %s\n", options->config_file);

        CmdOptions *ini_opt = cmd_options_create_defaults();
        cmd_options_parse_ini_file(ini_opt, options->config_file);

        merge_ini_into_options(options, ini_opt, parser);

        cmd_options_destroy(ini_opt);
    }

    const ParsingResult *val = validate_options(options);

    flag_parser_free(parser);
    return val;
}

void print_configuration(const CmdOptions *options) {
    if_verbose(VERBOSE_DEBUG,
               "--- Options ---\n\n"
               "Mode:                %s\n"
               "Input file:          %s\n"
               "Config file:         %s\n"
               "Verbosity:           %u\n"
               "Plot path:           %s\n"
               "Nodes:               %u\n"
               "Seed:                %d\n"
               "Area:                %d,%d (side %u)\n"
               "\n\n"
               "--- Algorithms ---\n"
               "Nearest Neighbor:    %s\n"
               "  plot:              %s\n"
               "  cost:              %s\n"
               "  time limit:        %.3f\n"
               "\n"
               "Extra Mileage:       %s\n"
               "  plot:              %s\n"
               "  cost:              %s\n"
               "  time limit:        %.3f\n"
               "\n"
               "VNS:                 %s\n"
               "  plot:              %s\n"
               "  cost:              %s\n"
               "  kicks MIN-MAX:     %u-%u\n"
               "  kicks reps:        %u\n"
               "  max stagnation:    %u\n"
               "  time limit:        %.3f\n"
               "\n"
               "Tabu Search:         %s\n"
               "  plot:              %s\n"
               "  cost:              %s\n"
               "  tenure, MIN-MAX:   %u-%u\n"
               "  max stagnation:    %u\n"
               "  time limit:        %.3f\n"
               "\n"
               "GRASP:               %s\n"
               "  plot:              %s\n"
               "  cost:              %s\n"
               "  RCL size:          %d\n"
               "  probability:       %.3f\n"
               "  max stagnation:    %d\n"
               "  time limit:        %.3f\n"
               "\n"
               "Genetic Algorithm:   %s\n"
               "  plot:              %s\n"
               "  cost:              %s\n"
               "  pop size:          %d\n"
               "  elite count:       %d\n"
               "  mutation rate:     %.3f\n"
               "  cut ratio MIN-MAX: %d-%d\n"
               "  time limit:        %.3f\n"
               "--------------\n",
               (options->inst.mode == TSP_INPUT_MODE_FILE ? "FILE" : "RANDOM"),
               options->inst.input_file ? options->inst.input_file : "(none)",
               options->config_file ? options->config_file : "(none)",
               options->verbosity,
               options->plots_path ? options->plots_path : "./",
               options->inst.number_of_nodes,
               options->inst.seed,
               options->inst.generation_area.x_square,
               options->inst.generation_area.y_square,
               options->inst.generation_area.square_side,

               options->nn_params.enable ? "ENABLED" : "DISABLED",
               options->nn_params.plot_file ? options->nn_params.plot_file : "(none)",
               options->nn_params.cost_file ? options->nn_params.cost_file : "(none)",
               options->nn_params.time_limit,

               options->em_params.enable ? "ENABLED" : "DISABLED",
               options->em_params.plot_file ? options->em_params.plot_file : "(none)",
               options->em_params.cost_file ? options->em_params.cost_file : "(none)",
               options->em_params.time_limit,

               options->vns_params.enable ? "ENABLED" : "DISABLED",
               options->vns_params.plot_file ? options->vns_params.plot_file : "(none)",
               options->vns_params.cost_file ? options->vns_params.cost_file : "(none)",
               options->vns_params.min_k,
               options->vns_params.max_k,
               options->vns_params.kick_repetitions,
               options->vns_params.max_stagnation,
               options->vns_params.time_limit,

               options->tabu_params.enable ? "ENABLED" : "DISABLED",
               options->tabu_params.plot_file ? options->tabu_params.plot_file : "(none)",
               options->tabu_params.cost_file ? options->tabu_params.cost_file : "(none)",
               options->tabu_params.min_tenure,
               options->tabu_params.max_tenure,
               options->tabu_params.max_stagnation,
               options->tabu_params.time_limit,

               options->grasp_params.enable ? "ENABLED" : "DISABLED",
               options->grasp_params.plot_file ? options->grasp_params.plot_file : "(none)",
               options->grasp_params.cost_file ? options->grasp_params.cost_file : "(none)",
               options->grasp_params.rcl_size,
               options->grasp_params.probability,
               options->grasp_params.max_stagnation,
               options->grasp_params.time_limit,

               options->genetic_params.enable ? "ENABLED" : "DISABLED",
               options->genetic_params.plot_file ? options->genetic_params.plot_file : "(none)",
               options->genetic_params.cost_file ? options->genetic_params.cost_file : "(none)",
               options->genetic_params.population_size,
               options->genetic_params.elite_count,
               options->genetic_params.mutation_rate,
               options->genetic_params.crossover_cut_min_ratio,
               options->genetic_params.crossover_cut_max_ratio,
               options->genetic_params.time_limit
    );
}
