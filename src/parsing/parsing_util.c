#include "parsing_util.h"
#include "constants.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

ParsingResult parse_long(const char *arg, long *parsed) {
    char *end;
    errno = 0;
    long val = strtol(arg, &end, 10);
    if (errno || *end != '\0') return PARSING_ERROR;
    *parsed = val;
    return SUCCESS;
}

ParsingResult parse_float(const char *arg, float *parsed) {
    char *end;
    errno = 0;
    float val = strtof(arg, &end);
    if (errno || *end != '\0') return PARSING_ERROR;
    *parsed = val;
    return SUCCESS;
}

ParsingResult parse_bool(const char *arg, bool *parsed) {
    if (!strcasecmp(arg, "true") || strcmp(arg, "1") == 0) {
        *parsed = true;
        return SUCCESS;
    }
    if (!strcasecmp(arg, "false") || strcmp(arg, "0") == 0) {
        *parsed = false;
        return SUCCESS;
    }
    return PARSING_ERROR;
}

ParsingResult parse_flag(const char *flag, const char *value, TspParams *params) {
    const int size = sizeof(tsp_flags) / sizeof(tsp_flags[0]);

    printf("%s\n", flag);
    printf("%s\n", value);

    for (int i = 0; i < size; i++) {
        ParsingResult result = tsp_flags[i].parse(value,params);
        if(result != SUCCESS){
          return result;
        }
    }

    return SUCCESS;
}
