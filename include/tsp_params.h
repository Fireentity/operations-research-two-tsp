#ifndef TSP_PARAMS_H
#define TSP_PARAMS_H

#include <stdbool.h>

#include "enums.h"

typedef struct
{
    long x_square;
    long y_square;
    long square_side;
} Rectangle;

typedef struct
{
    long number_of_nodes;
    long seed;
    Rectangle generation_area;
    bool help;
} TspParams;

typedef struct
{
    const char* label;
    const ParsingResult (*parse)(TspParams* self, const char* arg);
    const bool mandatory;
} TspParam;


ParsingResult parse_flag(const TspParam* tsp_flags, int tsp_flag_size, const char* flag, const char* value,
                         TspParams* params);
ParsingResult parse_long(const char* arg, long* parsed);
ParsingResult parse_float(const char* arg, float* parsed);
ParsingResult parse_bool(const char* arg, bool* parsed);
ParsingResult set_nodes(TspParams* self, const char* arg);
ParsingResult set_seed(TspParams* self, const char* arg);
ParsingResult set_x_square(TspParams* self, const char* arg);
ParsingResult set_y_square(TspParams* self, const char* arg);
ParsingResult set_square_side(TspParams* self, const char* arg);
ParsingResult set_help(TspParams* self, const char* arg);
#endif //TSP_PARAMS_H
