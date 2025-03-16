#include <cmd_options.h>
#include <c_util.h>
#include <flag.h>
#include <parsing_result.h>
#include <parsing_util.h>
#include <stdlib.h>
#include <string.h>

CmdOptions* init_cmd_options()
{
    const CmdOptions cmd_options = {
        .generation_area = {
            .square_side = 0,
            .x_square = 0,
            .y_square = 0,
        },
        .help = false,
        .number_of_nodes = 0,
        .seed = 0,
        .time_limit = 0
    };
    return malloc_from_stack(&cmd_options, sizeof(cmd_options));
}

ParsingResult parse_cli(CmdOptions* cmd_options, const char** const argv, const int argc)
{
    const Flag* tsp_flags[] = {
        init_flag("--nodes", 1, set_nodes, true),
        init_flag("--seed", 1, set_seed, false),
        init_flag("--x-square", 1, set_x_square, true),
        init_flag("--y-square", 1, set_y_square, true),
        init_flag("--square-side", 1, set_square_side, true),
        init_flag("--seconds", 1, set_time_limit, false),
        init_flag("--help", 0, set_help, false)
    };
    return parse_flags(cmd_options, tsp_flags, 7, argc, argv);
}

ParsingResult set_nodes(CmdOptions* cmd_options, const char** arg)
{
    return parse_unsigned_int(*(arg+1), &cmd_options->number_of_nodes);
}

ParsingResult set_seed(CmdOptions* cmd_options, const char** arg)
{
    return parse_int(*(arg+1), &cmd_options->seed);
}

ParsingResult set_x_square(CmdOptions* cmd_options, const char** arg)
{
    return parse_int(*(arg+1), &cmd_options->generation_area.x_square);
}

ParsingResult set_y_square(CmdOptions* cmd_options, const char** arg)
{
    return parse_int(*(arg+1), &cmd_options->generation_area.y_square);
}

ParsingResult set_square_side(CmdOptions* cmd_options, const char** arg)
{
    return parse_unsigned_int(*(arg+1), &cmd_options->generation_area.square_side);
}

ParsingResult set_help(CmdOptions* cmd_options, const char** arg)
{
    cmd_options->help = true;
    return PARSE_SUCCESS;
}

ParsingResult set_time_limit(CmdOptions* cmd_options, const char** arg)
{
    return parse_unsigned_int(*(arg+1), &cmd_options->time_limit);
}
