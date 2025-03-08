#include "tsp_params.h"
#include "parsing_util.h"
#include "enums.h"
#include "flag.h"
#include <stdbool.h>

ParsingResult parse_flag(const Flag* flag,
                         const char** argv,
                         TspParams* params,
                         int* index)
{
    return flag->parse(flag, argv, params, index);
}

ParsingResult set_nodes(TspParams* self, const char* arg)
{
    return parse_unsigned_int(arg, &self->number_of_nodes);
}

ParsingResult set_seed(TspParams* self, const char* arg)
{
    return parse_int(arg, &self->seed);
}

ParsingResult set_x_square(TspParams* self, const char* arg)
{
    return parse_int(arg, &self->generation_area.x_square);
}

ParsingResult set_y_square(TspParams* self, const char* arg)
{
    return parse_int(arg, &self->generation_area.y_square);
}

ParsingResult set_square_side(TspParams* self, const char* arg)
{
    return parse_unsigned_int(arg, &self->generation_area.square_side);
}

ParsingResult set_help(TspParams* self)
{
    self->help = true;
    return PARSE_SUCCESS;
}
