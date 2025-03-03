#include "parsing_util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *label;
    long *value;
} CommandLineFlag;

long parse_long(const char *str, int *error) {
    char *end;
    const long val = strtol(str, &end, 10);
    *error = *str == '\0' || *end != '\0';
    return val;
}

bool parse_arg(const char *argv, const char *flag, const char * value, long *arg) {
    if (!strcmp(argv, flag)) {
        return 1;
    }
    int err;
    *arg = parse_long(value, &err);
    return !err;
}

ParsingResult parse_flag(const char *flag, const char *value, TspParams *params) {
    const CommandLineFlag tsp_flags[] = {
        {"-nodes", &params->number_of_nodes},
        {"-seed", &params->seed},
        {"-x_square", &params->rectangle.x_square},
        {"-y_square", &params->rectangle.y_square},
        {"-square_side", &params->rectangle.square_side},
        {"-help", &params->help}
    };
    const int size = sizeof(tsp_flags) / sizeof(tsp_flags[0]);

    printf("%s\n", flag);
    printf("%s\n", value);

    for (int i = 0; i < size; i++) {
        if (!parse_arg(flag, tsp_flags[i].label, value, tsp_flags[i].value)) {
            return PARSING_ERROR;
        }
    }

    return SUCCESS;
}
