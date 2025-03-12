#ifndef TSP_PARAMS_H
#define TSP_PARAMS_H
#include <parsing_result.h>
#include <parsing_util.h>
#include <stdbool.h>
#include <tsp_instance.h>

typedef struct
{
    int x_square;
    int y_square;
    unsigned int square_side;
} Rectangle;

struct CmdOptions
{
    unsigned int number_of_nodes;
    int seed;
    Rectangle generation_area;
    bool help;
    unsigned int time_limit;
};

CmdOptions* init_cmd_options();
void parse_cli(CmdOptions* cmd_options, const char** argv, int argc);
ParsingResult set_nodes(CmdOptions* cmd_options, const char** arg);
ParsingResult set_seed(CmdOptions* cmd_options, const char** arg);
ParsingResult set_x_square(CmdOptions* cmd_options, const char** arg);
ParsingResult set_y_square(CmdOptions* cmd_options, const char** arg);
ParsingResult set_square_side(CmdOptions* cmd_options, const char** arg);
ParsingResult set_help(CmdOptions* cmd_options, const char** arg);
ParsingResult set_time_limit(CmdOptions* cmd_options, const char** arg);

#endif //TSP_PARAMS_H
