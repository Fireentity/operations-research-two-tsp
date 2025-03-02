#include "parsing_util.h"
#include <stdbool.h>
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

bool parse_arg(const char *argv, const char *flag, long &arg) {
    if (strcmp(argv, flag) != 0) {
        return 1;
    }
    int err;
    arg = parse_long(argv, &err);
    return err ? false : true;
}

ParsingResult parse_flag(const char *argv, TspParams *params) {
    const CommandLineFlag tsp_flags[] = {
        {"-nodes", &params->number_of_nodes},
        {"-seed", &params->seed},
        {"-x_square", &params->rectangle.x_square},
        {"-y_square", &params->rectangle.y_square},
        {"-square_side", &params->rectangle.square_side},
        {"-help", &params->help}
    };
    const int size = sizeof(tsp_flags) / sizeof(tsp_flags[0]);

    for (int i = 0; i < size; i++) {
        if (!parse_arg(argv, tsp_flags[i].label, *tsp_flags[i].value)) {
            return PARSING_ERROR;
        }
    }

    return SUCCESS;
}
