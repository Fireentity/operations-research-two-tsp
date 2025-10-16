#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <parsing_util.h>

const ParsingResult* parse_string(const char* arg, const char** parsed) {
    if (!arg) return MISSING_VALUE->of(MISSING_VALUE,arg);
    const unsigned long arg_len = strlen(arg)+1;
    *parsed = malloc(arg_len);
     memcpy((void*)*parsed, arg, arg_len);
    return SUCCESS;
}

// Parse an unsigned integer
const ParsingResult* parse_unsigned_int(const char* arg, unsigned int* parsed){
    if (!arg) return MISSING_VALUE->of(MISSING_VALUE,arg);

    char* end;
    errno = 0;
    const unsigned int val = strtoul(arg, &end, 10);

    if (errno || *end != '\0' || val > UINT_MAX) // Check for conversion errors or overflow
        return WRONG_VALUE_TYPE;

    *parsed = (unsigned int)val;
    return SUCCESS;
}

// Parse a signed integer
const ParsingResult* parse_int(const char* arg, int* parsed){
    if (!arg) return MISSING_VALUE;

    char* end;
    errno = 0;
    const long val = strtol(arg, &end, 10);

    if (errno || *end != '\0' || val < INT_MIN || val > INT_MAX) // Check for conversion errors or range violations
        return WRONG_VALUE_TYPE;

    *parsed = (int)val;
    return SUCCESS;
}

// Parse a float
const ParsingResult* parse_float(const char* arg, float* parsed){
    if (!arg) return MISSING_VALUE;

    char* end;
    errno = 0;
    const float val = strtof(arg, &end);

    if (errno || *end != '\0') {
        return WRONG_VALUE_TYPE;
    }

    *parsed = val;
    return SUCCESS;
}

// Parse a boolean
const ParsingResult* parse_bool(const char* arg, bool* parsed){
    if (!arg) return MISSING_VALUE;

    if (!strcasecmp(arg, "true") || strcmp(arg, "1") == 0){
        *parsed = true;
        return SUCCESS;
    }

    if (!strcasecmp(arg, "false") || strcmp(arg, "0") == 0){
        *parsed = false;
        return SUCCESS;
    }

    return WRONG_VALUE_TYPE;
}
