#include "parsing_util.h"

static ParsingResult set_nodes(const char * arg, TspParams *p) { return parse_long(arg,&p->number_of_nodes); }
static ParsingResult set_seed(const char * arg, TspParams *p) { return parse_long(arg,&p->seed); }
static ParsingResult set_x_square(const char * arg, TspParams *p) { return parse_long(arg,&p->rectangle.x_square); }
static ParsingResult set_y_square(const char * arg, TspParams *p) { return parse_long(arg,&p->rectangle.y_square); }
static ParsingResult set_square_side(const char * arg, TspParams *p) { return parse_long(arg,&p->rectangle.square_side); }
static ParsingResult set_help(const char * arg, TspParams *p) { return parse_bool(arg,&p->help); }

const CommandLineFlag tsp_flags[] = {
    {"--nodes", set_nodes, true},
    {"--seed", set_seed, false},
    {"--x_square", set_x_square, true},
    {"--y_square", set_y_square, true},
    {"--square_side", set_square_side, true},
    {"--help", set_help, false},
};