#include "cmd_options.h"
#include "parsing_util.h"
#include "enums.h"
#include <stdbool.h>

static CmdOptions cmd_options = {
    .generation_area = {
        .square_side = 0,
        .x_square = 0,
        .y_square = 0,
    },
    .help = false,
    .number_of_nodes = 0,
    .seed = 0,
    .seconds = 0
};

CmdOptions get_cmd_options()
{
    return cmd_options;
}

ParsingResult set_nodes(const char* arg)
{
    return parse_unsigned_int(arg, &cmd_options.number_of_nodes);
}

ParsingResult set_seed(const char* arg)
{
    return parse_int(arg, &cmd_options.seed);
}

ParsingResult set_x_square(const char* arg)
{
    return parse_int(arg, &cmd_options.generation_area.x_square);
}

ParsingResult set_y_square(const char* arg)
{
    return parse_int(arg, &cmd_options.generation_area.y_square);
}

ParsingResult set_square_side(const char* arg)
{
    return parse_unsigned_int(arg, &cmd_options.generation_area.square_side);
}

ParsingResult set_help()
{
    cmd_options.help = true;
    return PARSE_SUCCESS;
}

ParsingResult set_seconds(const char* arg)
{
    return parse_unsigned_int(arg, &cmd_options.seconds);
}
