#include "parsing_util.h"

static ParsingResult set_nodes(const char* arg, TspParams* p)
{
    return parse_long(arg, &p->number_of_nodes);
}

static ParsingResult set_seed(const char* arg, TspParams* p)
{
    return parse_long(arg, &p->seed);
}

static ParsingResult set_x_square(const char* arg, TspParams* p)
{
    return parse_long(arg, &p->generation_area.x_square);
}

static ParsingResult set_y_square(const char* arg, TspParams* p)
{
    return parse_long(arg, &p->generation_area.y_square);
}

static ParsingResult set_square_side(const char* arg, TspParams* p)
{
    return parse_long(arg, &p->generation_area.square_side);
}

static ParsingResult set_help(const char* arg, TspParams* p)
{
    return parse_bool(arg, &p->help);
}

const TspParam tsp_flags[] = {
    {.label = "--nodes", .parse = set_nodes, .mandatory = true},
    {.label = "--seed", .parse = set_seed, .mandatory = false},
    {.label = "--x_square", .parse = set_x_square, .mandatory = true},
    {.label = "--y_square", .parse = set_y_square, .mandatory = true},
    {.label = "--square_side", .parse = set_square_side, .mandatory = true},
    {.label = "--help", .parse = set_help, .mandatory = false},
};
