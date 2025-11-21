#include "cmd_options.h"
#include "cmd_option_internal.h"
#include "ini.h"
#include "parsing_util.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct {
    CmdOptions *opt;
} IniContext;

typedef enum {
    TYPE_UINT,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING
} IniType;

typedef struct {
    const char *section;
    const char *name;
    IniType type;
    size_t offset;
} IniMapping;

static const IniMapping mappings[] = {
    {"general", "verbosity", TYPE_UINT, offsetof(CmdOptions, verbosity)},
    {"general", "plots_path", TYPE_STRING, offsetof(CmdOptions, plots_path)},

    {"tsp", "file", TYPE_STRING, offsetof(CmdOptions, tsp.input_file)},
    {"tsp", "nodes", TYPE_UINT, offsetof(CmdOptions, tsp.number_of_nodes)},
    {"tsp", "seed", TYPE_INT, offsetof(CmdOptions, tsp.seed)},
    {"tsp", "seconds", TYPE_FLOAT, offsetof(CmdOptions, tsp.time_limit)},
    {"tsp", "x-square", TYPE_INT, offsetof(CmdOptions, tsp.generation_area.x_square)},
    {"tsp", "y-square", TYPE_INT, offsetof(CmdOptions, tsp.generation_area.y_square)},
    {"tsp", "square-side", TYPE_UINT, offsetof(CmdOptions, tsp.generation_area.square_side)},

    {"nn", "enabled", TYPE_BOOL, offsetof(CmdOptions, nn_params.enable)},
    {"nn", "plot_file", TYPE_STRING, offsetof(CmdOptions, nn_params.plot_file)},
    {"nn", "cost_file", TYPE_STRING, offsetof(CmdOptions, nn_params.cost_file)},

    {"vns", "enabled", TYPE_BOOL, offsetof(CmdOptions, vns_params.enable)},
    {"vns", "kick-repetitions", TYPE_UINT, offsetof(CmdOptions, vns_params.kick_repetitions)},
    {"vns", "n-opt", TYPE_UINT, offsetof(CmdOptions, vns_params.n_opt)},
    {"vns", "plot_file", TYPE_STRING, offsetof(CmdOptions, vns_params.plot_file)},
    {"vns", "cost_file", TYPE_STRING, offsetof(CmdOptions, vns_params.cost_file)},

    {"tabu", "enabled", TYPE_BOOL, offsetof(CmdOptions, tabu_params.enable)},
    {"tabu", "tenure", TYPE_UINT, offsetof(CmdOptions, tabu_params.tenure)},
    {"tabu", "max-stagnation", TYPE_UINT, offsetof(CmdOptions, tabu_params.max_stagnation)},
    {"tabu", "plot_file", TYPE_STRING, offsetof(CmdOptions, tabu_params.plot_file)},
    {"tabu", "cost_file", TYPE_STRING, offsetof(CmdOptions, tabu_params.cost_file)},

    {"grasp", "enabled", TYPE_BOOL, offsetof(CmdOptions, grasp_params.enable)},
    {"grasp", "p1", TYPE_FLOAT, offsetof(CmdOptions, grasp_params.p1)},
    {"grasp", "p2", TYPE_FLOAT, offsetof(CmdOptions, grasp_params.p2)},
    {"grasp", "plot_file", TYPE_STRING, offsetof(CmdOptions, grasp_params.plot_file)},
    {"grasp", "cost_file", TYPE_STRING, offsetof(CmdOptions, grasp_params.cost_file)},
};

static void handle_string(const char *value, char **dest) {
    if (*dest) {
        free(*dest);
        *dest = NULL;
    }
    parse_string(value, dest);
}

static int config_ini_handler(void *user, const char *section, const char *name, const char *value) {
    const IniContext *ctx = (IniContext *) user;
    CmdOptions *opt = ctx->opt;

    if (strcmp(section, "tsp") == 0 && strcmp(name, "mode") == 0) {
        opt->tsp.mode = strcmp(value, "file") == 0 ? TSP_INPUT_MODE_FILE : TSP_INPUT_MODE_RANDOM;
        return 1;
    }

    const size_t n = sizeof(mappings) / sizeof(mappings[0]);
    for (size_t i = 0; i < n; i++) {
        const IniMapping *m = &mappings[i];

        if (strcmp(section, m->section) == 0 &&
            strcmp(name, m->name) == 0) {
            void *field = (char *) opt + m->offset;

            switch (m->type) {
                case TYPE_UINT:
                    parse_uint(value, field);
                    break;
                case TYPE_INT:
                    parse_int(value, field);
                    break;
                case TYPE_FLOAT:
                    parse_float(value, field);
                    break;
                case TYPE_BOOL:
                    *(bool *) field = strcmp(value, "true") == 0;
                    break;
                case TYPE_STRING:
                    handle_string(value, field);
                    break;
            }
            return 1;
        }
    }

    return 1;
}

void cmd_options_parse_ini_file(CmdOptions *dest, const char *filename) {
    IniContext ctx = {dest};
    ini_parse(filename, config_ini_handler, &ctx);
}