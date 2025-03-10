#include "flag.h"
#include "enums.h"
#include <stdlib.h>
#include <string.h>
#include "c_util.h"

ParsingResult parse(const Flag* flag,
                         const char** argv,
                         int* index)
{
    const ParsingResult result = flag->parse_fuction(argv);
    *index += flag->number_of_params;
    return result;
}


const Flag* init_flag(const char* label,
                      const unsigned int number_of_params,
                      ParsingResult (*const param_supplier)(const char** arg),
                      const bool mandatory
)
{
    Flag* flag_ptr = malloc(sizeof(Flag));
    check_alloc(flag_ptr);
    const Flag flag = {
        .number_of_params = number_of_params,
        .parse_fuction = param_supplier,
        .label = label,
        .mandatory = mandatory
    };
    memcpy(flag_ptr, &flag, sizeof(flag));

    return flag_ptr;
}
