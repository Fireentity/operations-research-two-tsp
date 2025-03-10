#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd_options.h"
#include "flag.h"

ParsingResult parse_unsigned_int(const char* arg, unsigned int* parsed)
{
    char* end;
    errno = 0;
    const unsigned long val = strtoul(arg, &end, 10);
    if (errno || *end != '\0' || val > UINT_MAX)
        return PARSE_WRONG_VALUE_TYPE;
    *parsed = (unsigned int)val;
    return PARSE_SUCCESS;
}

ParsingResult parse_int(const char* arg, int* parsed)
{
    char* end;
    errno = 0;
    const long val = strtol(arg, &end, 10);
    if (errno || *end != '\0' || val < INT_MIN || val > INT_MAX)
        return PARSE_WRONG_VALUE_TYPE;
    *parsed = (int)val;
    return PARSE_SUCCESS;
}

ParsingResult parse_float(const char* arg, float* parsed)
{
    char* end;
    errno = 0;
    const float val = strtof(arg, &end);
    if (errno || *end != '\0') return PARSE_WRONG_VALUE_TYPE;
    *parsed = val;
    return PARSE_SUCCESS;
}

ParsingResult parse_bool(const char* arg, bool* parsed)
{
    if (!strcasecmp(arg, "true") || strcmp(arg, "1") == 0)
    {
        *parsed = true;
        return PARSE_SUCCESS;
    }
    if (!strcasecmp(arg, "false") || strcmp(arg, "0") == 0)
    {
        *parsed = false;
        return PARSE_SUCCESS;
    }
    return PARSE_WRONG_VALUE_TYPE;
}
