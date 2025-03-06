#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "parsing_util.h"

ParsingResult set_nodes(const char * arg, TspParams *p) { return parse_long(arg,&p->number_of_nodes); }
ParsingResult set_seed(const char * arg, TspParams *p) { return parse_long(arg,&p->seed); }
ParsingResult set_x_square(const char * arg, TspParams *p) { return parse_long(arg,&p->rectangle.x_square); }
ParsingResult set_y_square(const char * arg, TspParams *p) { return parse_long(arg,&p->rectangle.y_square); }
ParsingResult set_square_side(const char * arg, TspParams *p) { return parse_long(arg,&p->rectangle.square_side); }
ParsingResult set_help(const char * arg, TspParams *p) { return parse_bool(arg,&p->help); }

const CommandLineFlag tsp_flags[] = {
    {"--nodes", set_nodes},
    {"--seed", set_seed},
    {"--x_square", set_x_square},
    {"--y_square", set_y_square},
    {"--square_side", set_square_side},
    {"--help", set_help},
};

#endif //CONSTANTS_H
