#include <cmd_options.h>
#include <c_util.h>
#include <flag.h>
#include <parsing_result.h>
#include <parsing_util.h>

#include "flag_parser.h"
#include "ini.h"

CmdOptions* init_cmd_options() {
    return calloc(1, sizeof(CmdOptions));
}

void free_cmd_option(CmdOptions* cmd_options) {
    if (!cmd_options) return;
    if (cmd_options->config_file) {
        free((void*)cmd_options->config_file);
    }
    free(cmd_options);
}

const ParsingResult* set_config_label(CmdOptions* cmd_options, const char** arg) {
    return parse_string(*(arg + 1), &cmd_options->config_file);
}

const ParsingResult* set_nodes(CmdOptions* cmd_options, const char** arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->number_of_nodes);
}

const ParsingResult* set_seed(CmdOptions* cmd_options, const char** arg) {
    return parse_int(*(arg + 1), &cmd_options->seed);
}

const ParsingResult* set_x_square(CmdOptions* cmd_options, const char** arg) {
    return parse_int(*(arg + 1), &cmd_options->generation_area.x_square);
}

const ParsingResult* set_y_square(CmdOptions* cmd_options, const char** arg) {
    return parse_int(*(arg + 1), &cmd_options->generation_area.y_square);
}

const ParsingResult* set_square_side(CmdOptions* cmd_options, const char** arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->generation_area.square_side);
}

const ParsingResult* set_help(CmdOptions* cmd_options, const char** arg) {
    cmd_options->help = true;
    return SUCCESS;
}

const ParsingResult* set_vns(CmdOptions* cmd_options, const char** arg) {
    cmd_options->variable_neighborhood_search = true;
    return SUCCESS;
}

const ParsingResult* set_kick_repetitions(CmdOptions* cmd_options, const char** arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->kick_repetitions);
}

const ParsingResult* set_tenure(CmdOptions* cmd_options, const char** arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->tenure);
}

const ParsingResult* set_max_stagnation(CmdOptions* cmd_options, const char** arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->max_stagnation);
}

const ParsingResult* set_p1(CmdOptions* cmd_options, const char** arg) {
    return parse_float(*(arg + 1), &cmd_options->p1);
}

const ParsingResult* set_p2(CmdOptions* cmd_options, const char** arg) {
    return parse_float(*(arg + 1), &cmd_options->p2);
}

const ParsingResult* set_time_limit(CmdOptions* cmd_options, const char** arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->time_limit);
}

const ParsingResult* set_n_opt(CmdOptions* cmd_options, const char** arg) {
    return parse_unsigned_int(*(arg + 1), &cmd_options->n_opt);
}

const ParsingResult* set_nearest_neighbor(CmdOptions* cmd_options, const char** arg) {
    cmd_options->nearest_neighbor = true;
    return SUCCESS;
}

const ParsingResult* set_tabu_search(CmdOptions* cmd_options, const char** arg) {
    cmd_options->tabu_search = true;
    return SUCCESS;
}

const ParsingResult* set_grasp(CmdOptions* cmd_options, const char** arg) {
    cmd_options->grasp = true;
    return SUCCESS;
}

static struct FlagsArray* create_flags_array(const Flag** data, const size_t count) {
    struct FlagsArray* f_a = malloc(sizeof(struct FlagsArray));
    if (!f_a) return NULL;

    f_a->flags = memdup(data, sizeof(Flag*) * count);
    f_a->number_of_flags = count;
    return f_a;
}

struct FlagsArray* init_flags_array() {
    const Flag* vns_children_data[] = {
        init_flag("--kick-repetitions", 1, set_kick_repetitions, true),
        init_flag("--n-opt", 1, set_n_opt, true)
    };
    struct FlagsArray* vns_children = create_flags_array(vns_children_data, ARRAY_SIZE(vns_children_data));

    const Flag* tabu_children_data[] = {
        init_flag("--tenure", 1, set_tenure, true),
        init_flag("--max-stagnation", 1, set_max_stagnation, true)
    };
    struct FlagsArray* tabu_children = create_flags_array(tabu_children_data, ARRAY_SIZE(tabu_children_data));

    const Flag* grasp_children_data[] = {
        init_flag("--p1", 1, set_p1, true),
        init_flag("--p2", 1, set_p2, true),
    };
    struct FlagsArray* grasp_children = create_flags_array(grasp_children_data, ARRAY_SIZE(grasp_children_data));

    const Flag* tsp_flags_data[] = {
        init_flag("--nodes", 1, set_nodes, true),
        init_flag("--seed", 1, set_seed, false),
        init_flag("--x-square", 1, set_x_square, true),
        init_flag("--y-square", 1, set_y_square, true),
        init_flag("--square-side", 1, set_square_side, true),
        init_flag("--seconds", 1, set_time_limit, false),
        init_flag("--help", 0, set_help, false),
        init_flag_with_children("--vns", 0, set_vns, false, vns_children), // Passa il puntatore!
        init_flag("--nearest-neighbor", 0, set_nearest_neighbor, false),
        init_flag_with_children("--tabu-search", 0, set_tabu_search, false, tabu_children), // Passa il puntatore!
        init_flag_with_children("--grasp", 0, set_grasp, false, grasp_children) // Passa il puntatore!
    };

    return create_flags_array(tsp_flags_data, ARRAY_SIZE(tsp_flags_data));
}

#define SECTION_MATCH(s) !strcmp(section, s)
#define NAME_MATCH(n) !strcmp(name, n)

static int handler(void* user, const char* section, const char* name,
                   const char* value) {
    CmdOptions* cmd_options = user;
    const ParsingResult* result = SUCCESS;

    if (SECTION_MATCH("tsp")) {
        if (NAME_MATCH("nodes")) result = parse_unsigned_int(value, &cmd_options->number_of_nodes);
        else if (NAME_MATCH("seed")) result = parse_int(value, &cmd_options->seed);
        else if (NAME_MATCH("x-square")) result = parse_int(value, &cmd_options->generation_area.x_square);
        else if (NAME_MATCH("y-square")) result = parse_int(value, &cmd_options->generation_area.y_square);
        else if (NAME_MATCH("square-side"))
            result =
                parse_unsigned_int(value, &cmd_options->generation_area.square_side);
        else if (NAME_MATCH("seconds")) result = parse_unsigned_int(value, &cmd_options->time_limit);
        else if (NAME_MATCH("nearest-neighbor")) cmd_options->nearest_neighbor = (strcmp(value, "true") == 0);
        else return 0;
    }
    else if (SECTION_MATCH("vns")) {
        if (NAME_MATCH("enabled")) cmd_options->variable_neighborhood_search = (strcmp(value, "true") == 0);
        else if (NAME_MATCH("kick-repetitions")) result = parse_unsigned_int(value, &cmd_options->kick_repetitions);
        else if (NAME_MATCH("n-opt")) result = parse_unsigned_int(value, &cmd_options->n_opt);
        else return 0;
    }
    else if (SECTION_MATCH("tabu")) {
        if (NAME_MATCH("enabled")) cmd_options->tabu_search = (strcmp(value, "true") == 0);
        else if (NAME_MATCH("tenure")) result = parse_unsigned_int(value, &cmd_options->tenure);
        else if (NAME_MATCH("max-stagnation")) result = parse_unsigned_int(value, &cmd_options->max_stagnation);
        else return 0;
    }
    else if (SECTION_MATCH("grasp")) {
        if (NAME_MATCH("enabled")) cmd_options->grasp = (strcmp(value, "true") == 0);
        else if (NAME_MATCH("p1")) result = parse_float(value, &cmd_options->p1);
        else if (NAME_MATCH("p2")) result = parse_float(value, &cmd_options->p2);
        else return 0;
    }
    else return 0;

    return result->state == PARSE_SUCCESS;
}

void load_config(CmdOptions* cmd_options, const char** arg) {
    const Flag* config_flags[] = {
        init_flag("--config", 1, set_config_label, false)
    };

    struct FlagsArray* f_a = create_flags_array(config_flags, ARRAY_SIZE(config_flags));
    FlagParser* parser = init_flag_parser(f_a);
    const ParsingResult* parsing_result = parse_flags_with_parser(cmd_options, parser, arg + 1, true);
    if (parsing_result->state != PARSE_SUCCESS) {
        printf("%s", parsing_result->error_message);
        goto FREE;
    }
    if (cmd_options->config_file && ini_parse(cmd_options->config_file, handler, cmd_options) < 0) {
        printf("Can't open config file: '%s' \n", cmd_options->config_file);
    }
FREE:
    free_flag_parser(parser);
    free_flags_array(f_a);
}
