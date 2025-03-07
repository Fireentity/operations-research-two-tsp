#include "parsing_util.h"
#include "constants.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

ParsingResult parse_long(const char* arg, long* parsed)
{
    char* end;
    errno = 0;
    const long val = strtol(arg, &end, 10);
    if (errno || *end != '\0') return PARSING_ERROR;
    *parsed = val;
    return SUCCESS;
}

ParsingResult parse_float(const char* arg, float* parsed)
{
    char* end;
    errno = 0;
    const float val = strtof(arg, &end);
    if (errno || *end != '\0') return PARSING_ERROR;
    *parsed = val;
    return SUCCESS;
}

ParsingResult parse_bool(const char* arg, bool* parsed)
{
    if (!strcasecmp(arg, "true") || strcmp(arg, "1") == 0)
    {
        *parsed = true;
        return SUCCESS;
    }
    if (!strcasecmp(arg, "false") || strcmp(arg, "0") == 0)
    {
        *parsed = false;
        return SUCCESS;
    }
    return PARSING_ERROR;
}

ParsingResult parse_flag(const TspParam tsp_flags*,
    const int tsp_flag_size,
    const char* flag,
    const char* value,
    TspParams* params)
{
    printf("%s\n", flag);
    printf("%s\n", value);

    for (int i = 0; i < tsp_flag_size; i++)
    {
        const ParsingResult result = tsp_flags[i].parse(value, params);
        if (result != SUCCESS)
        {
            return result;
        }
    }

    return SUCCESS;
}
