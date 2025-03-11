#include <flag.h>
#include <c_util.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

ParsingResult parse(const CmdOptions* const cmd_options,
                    const Flag* flag,
                    const char** argv,
                    unsigned int* index)
{
    const ParsingResult result = flag->parse_function(cmd_options, argv);
    *index += flag->number_of_params;
    return result;
}


const Flag* init_flag(const char* label,
                      const unsigned int number_of_params,
                      ParsingResult (*const param_supplier)(const CmdOptions* cmd_options, const char** arg),
                      const bool mandatory
)
{
    Flag* flag_ptr = malloc(sizeof(Flag));
    check_alloc(flag_ptr);
    const Flag flag = {
        .number_of_params = number_of_params,
        .parse_function = param_supplier,
        .label = label,
        .mandatory = mandatory
    };
    memcpy(flag_ptr, &flag, sizeof(flag));

    return flag_ptr;
}
