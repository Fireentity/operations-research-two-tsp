#include "cmd_options.h"
#include "cmd_option_internal.h"
#include "ini.h"
#include "parsing_util.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "logger.h"

typedef struct {
    CmdOptions *opt;
} IniContext;

typedef enum {
    TYPE_INT,
    TYPE_UINT,
    TYPE_DOUBLE,
    TYPE_UDOUBLE,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_TSP_MODE
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

    {"tsp", "mode", TYPE_TSP_MODE, offsetof(CmdOptions, tsp.mode)},
    {"tsp", "file", TYPE_STRING, offsetof(CmdOptions, tsp.input_file)},
    {"tsp", "nodes", TYPE_UINT, offsetof(CmdOptions, tsp.number_of_nodes)},
    {"tsp", "seed", TYPE_INT, offsetof(CmdOptions, tsp.seed)},
    {"tsp", "x-square", TYPE_INT, offsetof(CmdOptions, tsp.generation_area.x_square)},
    {"tsp", "y-square", TYPE_INT, offsetof(CmdOptions, tsp.generation_area.y_square)},
    {"tsp", "square-side", TYPE_UINT, offsetof(CmdOptions, tsp.generation_area.square_side)},

    {"nn", "enabled", TYPE_BOOL, offsetof(CmdOptions, nn_params.enable)},
    {"nn", "plot_file", TYPE_STRING, offsetof(CmdOptions, nn_params.plot_file)},
    {"nn", "cost_file", TYPE_STRING, offsetof(CmdOptions, nn_params.cost_file)},
    {"nn", "seconds", TYPE_UDOUBLE, offsetof(CmdOptions, nn_params.time_limit)},

    {"vns", "enabled", TYPE_BOOL, offsetof(CmdOptions, vns_params.enable)},
    {"vns", "min_k", TYPE_UINT, offsetof(CmdOptions, vns_params.min_k)},
    {"vns", "max_k", TYPE_UINT, offsetof(CmdOptions, vns_params.max_k)},
    {"vns", "kick-repetitions", TYPE_UINT, offsetof(CmdOptions, vns_params.kick_repetitions)},
    {"vns", "max-stagnation", TYPE_UINT, offsetof(CmdOptions, vns_params.max_stagnation)},
    {"vns", "plot_file", TYPE_STRING, offsetof(CmdOptions, vns_params.plot_file)},
    {"vns", "cost_file", TYPE_STRING, offsetof(CmdOptions, vns_params.cost_file)},
    {"vns", "seconds", TYPE_UDOUBLE, offsetof(CmdOptions, vns_params.time_limit)},

    {"tabu", "enabled", TYPE_BOOL, offsetof(CmdOptions, tabu_params.enable)},
    {"tabu", "min-tenure", TYPE_UINT, offsetof(CmdOptions, tabu_params.min_tenure)},
    {"tabu", "max-tenure", TYPE_UINT, offsetof(CmdOptions, tabu_params.max_tenure)},
    {"tabu", "max-stagnation", TYPE_UINT, offsetof(CmdOptions, tabu_params.max_stagnation)},
    {"tabu", "plot_file", TYPE_STRING, offsetof(CmdOptions, tabu_params.plot_file)},
    {"tabu", "cost_file", TYPE_STRING, offsetof(CmdOptions, tabu_params.cost_file)},
    {"tabu", "seconds", TYPE_UDOUBLE, offsetof(CmdOptions, tabu_params.time_limit)},

    {"grasp", "enabled", TYPE_BOOL, offsetof(CmdOptions, grasp_params.enable)},
    {"grasp", "p1", TYPE_UDOUBLE, offsetof(CmdOptions, grasp_params.p1)},
    {"grasp", "p2", TYPE_UDOUBLE, offsetof(CmdOptions, grasp_params.p2)},
    {"grasp", "plot_file", TYPE_STRING, offsetof(CmdOptions, grasp_params.plot_file)},
    {"grasp", "cost_file", TYPE_STRING, offsetof(CmdOptions, grasp_params.cost_file)},
    {"grasp", "seconds", TYPE_UDOUBLE, offsetof(CmdOptions, grasp_params.time_limit)},
};

static void handle_string(const char *value, char **dest) {
    if (*dest) {
        free(*dest);
        *dest = NULL;
    }
    parse_string(value, dest);
}

static void handle_tsp_mode(const char *v, void *dst) {
    TspInputMode *m = dst;
    if (strcmp(v, "file") == 0) *m = TSP_INPUT_MODE_FILE;
    else *m = TSP_INPUT_MODE_RANDOM;
}

static int config_ini_handler(void *user, const char *section, const char *name, const char *value) {
    const IniContext *ctx = (IniContext *) user;
    CmdOptions *opt = ctx->opt;

    const size_t n = sizeof(mappings) / sizeof(mappings[0]);
    for (size_t i = 0; i < n; i++) {
        const IniMapping *m = &mappings[i];

        if (strcmp(section, m->section) == 0 &&
            strcmp(name, m->name) == 0) {
            void *field = (char *) opt + m->offset;

            switch (m->type) {
                case TYPE_INT:
                    parse_int(value, field);
                    break;
                case TYPE_UINT:
                    parse_uint(value, field);
                    break;
                case TYPE_DOUBLE:
                    parse_double(value, field);
                    break;
                case TYPE_UDOUBLE:
                    parse_udouble(value, field);
                    break;
                case TYPE_BOOL:
                    *(bool *) field = strcmp(value, "true") == 0;
                    break;
                case TYPE_STRING:
                    handle_string(value, field);
                    break;
                case TYPE_TSP_MODE:
                    handle_tsp_mode(value, field);
                    break;
            }
            return 1;
        }
    }

    return 1;
}

void cmd_options_parse_ini_file(CmdOptions *dest, const char *filename) {
    IniContext ctx = {dest};
    const int r = ini_parse(filename, config_ini_handler, &ctx);

    if_verbose(VERBOSE_DEBUG, "INI: parsing '%s' finished with code %d\n", filename, r);

    if (r == 0) {
        if_verbose(VERBOSE_INFO, "INI: file '%s' parsed successfully\n", filename);
    } else if (r == -1) {
        if_verbose(VERBOSE_INFO, "INI: could not open '%s'\n", filename);
    } else {
        if_verbose(VERBOSE_INFO, "INI: error at line %d in '%s'\n", r, filename);
    }
}
