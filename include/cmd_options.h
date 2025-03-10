#ifndef TSP_PARAMS_H
#define TSP_PARAMS_H

#include "enums.h"
#include <stdbool.h>

typedef struct
{
    int x_square;
    int y_square;
    unsigned int square_side;
} Rectangle;

typedef struct
{
    unsigned int number_of_nodes;
    int seed;
    Rectangle generation_area;
    bool help;
    unsigned int seconds;
} CmdOptions;

CmdOptions get_cmd_options();
ParsingResult set_nodes(const char** arg);
ParsingResult set_seed(const char** arg);
ParsingResult set_x_square(const char** arg);
ParsingResult set_y_square(const char** arg);
ParsingResult set_square_side(const char** arg);
ParsingResult set_help(const char** arg);
ParsingResult set_seconds(const char** arg);
#endif //TSP_PARAMS_H
