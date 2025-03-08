#ifndef TSP_PARAMS_H
#define TSP_PARAMS_H

#include "enums.h"
#include <stdbool.h>

typedef struct Flag Flag;

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
} TspParams;

ParsingResult parse_flag(const Flag* flag, const char** argv, TspParams* params, int* index);
ParsingResult parse_unsigned_int(const char* arg, unsigned int* parsed);
ParsingResult parse_float(const char* arg, float* parsed);
ParsingResult parse_bool(const char* arg, bool* parsed);
ParsingResult set_nodes(TspParams* self, const char* arg);
ParsingResult set_seed(TspParams* self, const char* arg);
ParsingResult set_x_square(TspParams* self, const char* arg);
ParsingResult set_y_square(TspParams* self, const char* arg);
ParsingResult set_square_side(TspParams* self, const char* arg);
ParsingResult set_help(TspParams* self);
#endif //TSP_PARAMS_H
