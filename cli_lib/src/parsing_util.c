#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <parsing_util.h>

ParsingResult parse_unsigned_int(const char* arg, unsigned int* parsed)
{
    if (!arg) return PARSE_MISSING_VALUE;
    char* end;
    errno = 0;
    const unsigned int val = strtoul(arg, &end, 10);
    if (errno || *end != '\0' || val > UINT_MAX) // Check conversion errors and overflow
        return PARSE_WRONG_VALUE_TYPE;
    *parsed = (unsigned int)val;
    return PARSE_SUCCESS;
}

ParsingResult parse_int(const char* arg, int* parsed)
{
    if (!arg) return PARSE_MISSING_VALUE;
    char* end;
    errno = 0;
    const long val = strtol(arg, &end, 10);
    if (errno || *end != '\0' || val < INT_MIN || val > INT_MAX) // Verify range and conversion
        return PARSE_WRONG_VALUE_TYPE;
    *parsed = (int)val;
    return PARSE_SUCCESS;
}

ParsingResult parse_float(const char* arg, float* parsed)
{
    if (!arg) return PARSE_MISSING_VALUE;
    char* end;
    errno = 0;
    const float val = strtof(arg, &end);
    if (errno || *end != '\0') // Ensure complete conversion
        return PARSE_WRONG_VALUE_TYPE;
    *parsed = val;
    return PARSE_SUCCESS;
}

ParsingResult parse_bool(const char* arg, bool* parsed)
{
    if (!arg) return PARSE_MISSING_VALUE;
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