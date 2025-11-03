#include "parsing_util.h"
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

const ParsingResult *parse_int(const char *s, int *dest) {
    char *endptr;

    errno = 0;

    const long val = strtol(s, &endptr, 10);

    if (endptr == s) {
        return WRONG_VALUE_TYPE;
    }

    if (errno == ERANGE) {
        return WRONG_VALUE_TYPE;
    }

    if (*endptr != '\0') {
        return WRONG_VALUE_TYPE;
    }

    if (val > INT_MAX || val < INT_MIN) {
        return WRONG_VALUE_TYPE;
    }

    *dest = (int) val;
    return SUCCESS;
}

const ParsingResult *parse_unsigned_int(const char *s, unsigned int *dest) {
    char *endptr;

    if (s[0] == '-') {
        return WRONG_VALUE_TYPE;
    }

    errno = 0;
    const unsigned long val = strtoul(s, &endptr, 10);

    if (endptr == s) {
        return WRONG_VALUE_TYPE;
    }

    if (errno == ERANGE) {
        return WRONG_VALUE_TYPE;
    }

    if (*endptr != '\0') {
        return WRONG_VALUE_TYPE;
    }

    if (val > UINT_MAX) {
        return WRONG_VALUE_TYPE;
    }

    *dest = (unsigned int) val;
    return SUCCESS;
}

const ParsingResult *parse_float(const char *s, float *dest) {
    char *endptr;
    errno = 0;

    const double val = strtod(s, &endptr);

    if (endptr == s) {
        return WRONG_VALUE_TYPE;
    }

    if (errno == ERANGE) {
        return WRONG_VALUE_TYPE;
    }

    if (*endptr != '\0') {
        return WRONG_VALUE_TYPE;
    }

    *dest = (float) val;
    return SUCCESS;
}
