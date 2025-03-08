#include "empty_flag.h"
#include "flag.h"
#include <stdlib.h>
#include <string.h>
#include "c_util.h"

static ParsingResult parse(const Flag* flag, const char** argv, TspParams* params, int* idx)
{
    const char* label = argv[*idx];

    if (strcmp(flag->label, label) != 0)
    {
        return PARSE_NON_MATCHING_LABEL;
    }
    const ParsingResult result = flag->empty_flag->param_supplier(params);
    return result;
}


const Flag* init_empty_flag(const char* label,
                            ParsingResult (*param_supplier)(TspParams* self),
                            const bool mandatory
)
{
    EmptyFlag* empty_flag_ptr = malloc(sizeof(EmptyFlag));
    check_alloc(empty_flag_ptr);
    const EmptyFlag empty_flag = {
        .param_supplier = param_supplier
    };
    memcpy(empty_flag_ptr, &empty_flag, sizeof(empty_flag));

    Flag* flag_ptr = malloc(sizeof(Flag));
    const Flag flag = {
        .parse = parse,
        .empty_flag = empty_flag_ptr,
        .label = label,
        .mandatory = mandatory
    };
    memcpy(flag_ptr, &flag, sizeof(flag));

    return flag_ptr;
}
