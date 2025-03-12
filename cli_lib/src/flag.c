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
    ParsingResult (* const parse_function)(void* options, const char** arg);
};

static bool is_mandatory(const Flag* const self) { return self->state->mandatory; }

static ParsingResult parse(const Flag* flag,
                           void* const options,
                           const char** argv,
                           unsigned int* index)
{
    const ParsingResult result = flag->state->parse_function(options, argv);
    *index += flag->state->number_of_params;
    return result;
}

const Flag* init_flag(const char* label,
                      const unsigned int number_of_params,
                      ParsingResult (*const param_supplier)(void* options, const char** arg),
                      const bool mandatory
)
{
    FlagState state = {
        .number_of_params = number_of_params,
        .parse_function = param_supplier,
        .label = label,
        .mandatory = mandatory
    };
    const Flag flag = {
        .state = MALLOC_FROM_STACK(state),
        .parse = parse,
        .is_mandatory = is_mandatory
    };
    return MALLOC_FROM_STACK(flag);
}
