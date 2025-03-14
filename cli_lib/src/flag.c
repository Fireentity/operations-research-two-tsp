#include <c_util.h>
#include <flag.h>
#include <parsing_result.h>
#include <stdlib.h>
#include <string.h>

struct FlagState
{
    const unsigned int number_of_params;
    const char* label;
    const bool mandatory;
    ParsingResult (* const parse_function)(CmdOptions* cmd_options, const char** arg);
};

static bool is_mandatory(const Flag* const self) { return self->state->mandatory; }

static int get_number_of_params(const Flag* const self) { return self->state->number_of_params; }

static ParsingResult parse(const Flag* flag,
                    CmdOptions* const cmd_options,
                    const char** argv,
                    unsigned int* index)
{
    if (strcasecmp(flag->state->label, argv[*index]) != 0) return PARSE_NON_MATCHING_LABEL;
    const ParsingResult result = flag->state->parse_function(cmd_options, argv+*index);
    if (result == PARSE_SUCCESS) *index += flag->state->number_of_params;
    return result;
}

const Flag* init_flag(const char* label,
                      const unsigned int number_of_params,
                      ParsingResult (*const param_function)(CmdOptions* cmd_option, const char** arg),
                      const bool mandatory
)
{
    FlagState state = {
        .number_of_params = number_of_params,
        .parse_function = param_function,
        .label = label,
        .mandatory = mandatory
    };
    Flag flag = {
        .state = MALLOC_FROM_STACK(state),
        .parse = parse,
        .is_mandatory = is_mandatory,
        .get_number_of_params = get_number_of_params
    };
    return MALLOC_FROM_STACK(flag);
}
