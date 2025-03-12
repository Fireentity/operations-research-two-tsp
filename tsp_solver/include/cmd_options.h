#ifndef TSP_PARAMS_H
#define TSP_PARAMS_H
#include <parsing_result.h>
#include <stdbool.h>
#include <tsp_instance.h>

typedef struct
{
    unsigned int number_of_nodes;
    int seed;
    TspGenerationArea generation_area;
    bool help;
    unsigned int time_limit;
} CmdOptions;

CmdOptions* init_cmd_options();
void parse_cli(CmdOptions* cmd_options, const char** argv, int argc);
ParsingResult set_nodes(void* options, const char** arg);
ParsingResult set_seed(void* options, const char** arg);
ParsingResult set_x_square(void* options, const char** arg);
ParsingResult set_y_square(void* options, const char** arg);
ParsingResult set_square_side(void* options, const char** arg);
ParsingResult set_help(void* options, const char** arg);
ParsingResult set_time_limit(void* options, const char** arg);

#endif //TSP_PARAMS_H
