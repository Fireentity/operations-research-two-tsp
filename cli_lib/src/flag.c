#include <flag.h>
#include <c_util.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

ParsingResult parse(const Flag* flag,
                    void* const options,
                    const char** argv,
                    unsigned int* index)
{
    const ParsingResult result = flag->parse_function(options, argv);
    *index += flag->number_of_params;
    return result;
}


const Flag* init_flag(const char* label,
                      const unsigned int number_of_params,
                      ParsingResult (*const param_supplier)(void* options, const char** arg),
                      const bool mandatory
)
{
    const Flag flag = {
        .number_of_params = number_of_params,
        .parse_function = param_supplier,
        .label = label,
        .mandatory = mandatory
    };
    return MALLOC_FROM_STACK(flag);
}
