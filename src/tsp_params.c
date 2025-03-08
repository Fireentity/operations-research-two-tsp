#include "tsp_params.h"
#include "parsing_util.h"
#include "enums.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct CommandFlag
{
    const char* const label;

    union
    {
        ParsingResult (*const parse_with_value)(TspParams* self, const char* arg);
        ParsingResult (*const parse_without_value)(TspParams* self);
    };

    ParsingResult (*const parse_flag)(const CommandFlag* param, const char** argv, TspParams* params, int* index);
    const bool mandatory;
};

ParsingResult parse_flag_with_value(const CommandFlag* param, const char** argv, TspParams* params, int* index);
ParsingResult parse_flag_without_value(const CommandFlag* param, const char** argv, TspParams* params, int* index);

const CommandFlag* initialize_command_flag_with_value(
    const char* label,
    ParsingResult (*const param_supplier)(TspParams* self, const char* arg),
    const bool mandatory
)
{
    const CommandFlag flag = {
        .label = label,
        .parse_with_value = param_supplier,
        .mandatory = mandatory,
        .parse_flag = parse_flag_with_value
    };
    CommandFlag* flag_ptr = malloc(sizeof(CommandFlag));
    memcpy(flag_ptr, &flag, sizeof(CommandFlag));
    return flag_ptr;
}

const CommandFlag* initialize_command_flag_without_value(
    const char* label,
    ParsingResult (*const parse)(TspParams* self),
    const bool mandatory
)
{
    const CommandFlag flag = {
        .label = label,
        .parse_without_value = parse,
        .mandatory = mandatory,
        .parse_flag = parse_flag_without_value
    };
    CommandFlag* flag_ptr = malloc(sizeof(CommandFlag));
    memcpy(flag_ptr, &flag, sizeof(CommandFlag));
    return flag_ptr;
}

bool is_command_flag_mandatory(const CommandFlag* command_flag)
{
    return command_flag->mandatory;
}

ParsingResult parse_flag_with_value(const CommandFlag* param,
                                    const char** argv,
                                    TspParams* params,
                                    int* index)
{
    const char* label = argv[*index];
    const char* value = argv[*index + 1];

    if (!strcmp(param->label, label))
    {
        return PARSE_NON_MATCHING_LABEL;
    }
    const ParsingResult result = param->parse_with_value(params, value);
    if (result == PARSE_SUCCESS) (*index)++;
    return result;
}

ParsingResult parse_flag_without_value(const CommandFlag* param,
                                       const char** argv,
                                       TspParams* params,
                                       int* index)
{
    const char* label = argv[*index];

    if (!strcmp(param->label, label))
    {
        return PARSE_NON_MATCHING_LABEL;
    }
    const ParsingResult result = param->parse_without_value(params);
    if (result != PARSE_SUCCESS) (*index)++;
    return result;
}

ParsingResult parse_flag(const CommandFlag* param,
                         const char** argv,
                         TspParams* params,
                         int* index)
{
    return param->parse_flag(param, argv, params, index);
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
