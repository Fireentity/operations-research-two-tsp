#include "single_flag.h"
#include "flag.h"
#include "c_util.h"
#include <stdlib.h>
#include <string.h>

static ParsingResult parse(const Flag* flag, const char** argv, int* idx)
{
    const char* label = argv[*idx];
    const char* value = argv[*idx + 1];

    if (strcmp(flag->label, label) != 0)
    {
        return PARSE_NON_MATCHING_LABEL;
    }
    const ParsingResult result = flag->single_flag->param_supplier(value);
    if (result == PARSE_SUCCESS) (*idx)++;
    return result;
}

const Flag* init_single_flag(const char* label,
                             ParsingResult (*const param_supplier)(const char* arg),
                             const bool mandatory
)
{
    SingleFlag* single_flag_ptr = malloc(sizeof(SingleFlag));
    check_alloc(single_flag_ptr);
    const SingleFlag single_flag = {
        .param_supplier = param_supplier
    };
    memcpy(single_flag_ptr, &single_flag, sizeof(single_flag));

    Flag* flag_ptr = malloc(sizeof(Flag));
    const Flag flag = {
        .parse = parse,
        .single_flag = single_flag_ptr,
        .label = label,
        .mandatory = mandatory
    };
    memcpy(flag_ptr, &flag, sizeof(flag));

    return flag_ptr;
}
