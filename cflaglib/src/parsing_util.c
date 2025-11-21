#include "parsing_util.h"
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

const ParsingResult *parse_string(const char *s, char **dest) {
    if (s == NULL) {
        *dest = NULL;
        return SUCCESS;
    }
    char *tmp = strdup(s);
    if (tmp == NULL) {
        return INTERNAL_ERROR;
    }

    *dest = tmp;

    return SUCCESS;
}

const ParsingResult *parse_int(const char *s, int *dest) {
    char *endptr;
    errno = 0;

    const long val = strtol(s, &endptr, 10);

    // Check if any digits were read
    if (endptr == s) {
        return WRONG_VALUE_TYPE;
    }

    // Check for overflow or underflow
    if (errno == ERANGE) {
        return WRONG_VALUE_TYPE;
    }

    // Check if the entire string was consumed
    if (*endptr != '\0') {
        return WRONG_VALUE_TYPE;
    }

    // Check if the long value fits into an int
    if (val > INT_MAX || val < INT_MIN) {
        return WRONG_VALUE_TYPE;
    }

    *dest = (int) val;
    return SUCCESS;
}

const ParsingResult *parse_uint(const char *s, unsigned int *dest) {
    char *endptr;

    // strtoul allows a negative sign, so we must check for it manually
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

const ParsingResult *parse_ufloat(const char *s, float *dest) {
    // Reuse the float parser
    const ParsingResult *tmp = parse_float(s, dest);
    if (tmp != SUCCESS) {
        return tmp;
    }

    // Add the specific check for "unsigned"
    if (*dest < 0.0f) {
        return WRONG_VALUE_TYPE;
    }

    return SUCCESS;
}