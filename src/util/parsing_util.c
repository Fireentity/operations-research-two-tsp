#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd_options.h"
#include "flag.h"
#include "parsing_util.h"


ParsingResult parse_unsigned_int(const char* arg, unsigned int* parsed)
{
    char* end;
    errno = 0;
    const unsigned long val = strtoul(arg, &end, 10);
    if (errno || *end != '\0' || val > UINT_MAX)
        return PARSE_WRONG_VALUE_TYPE;
    *parsed = (unsigned int) val;
    return PARSE_SUCCESS;
}

ParsingResult parse_int(const char *arg, int *parsed) {
    char *end;
    errno = 0;
    const long val = strtol(arg, &end, 10);
    if (errno || *end != '\0' || val < INT_MIN || val > INT_MAX)
        return PARSE_WRONG_VALUE_TYPE;
    *parsed = (int) val;
    return PARSE_SUCCESS;
}

ParsingResult parse_float(const char *arg, float *parsed) {
    char *end;
    errno = 0;
    const float val = strtof(arg, &end);
    if (errno || *end != '\0') return PARSE_WRONG_VALUE_TYPE;
    *parsed = val;
    return PARSE_SUCCESS;
}

ParsingResult parse_bool(const char *arg, bool *parsed) {
    if (!strcasecmp(arg, "true") || strcmp(arg, "1") == 0) {
        *parsed = true;
        return PARSE_SUCCESS;
    }
    if (!strcasecmp(arg, "false") || strcmp(arg, "0") == 0) {
        *parsed = false;
        return PARSE_SUCCESS;
    }
    return PARSE_WRONG_VALUE_TYPE;
}

// Function to parse the command-line arguments using flags.
void parse_command_line(
        const int argc,
        const char **argv) {
    if (argc < 2)
        PARSING_ABORT(PARSE_USAGE_ERROR);

    const Flag *tsp_flags[] = {
            init_single_flag("--nodes", set_nodes, true),
            init_single_flag("--seed", set_seed, false),
            init_single_flag("--x-square", set_x_square, true),
            init_single_flag("--y-square", set_y_square, true),
            init_single_flag("--square-side", set_square_side, true),
            init_single_flag("--seconds", set_seconds, false),
            init_empty_flag("--help", set_help, false)
    };

    const int tsp_flag_size = sizeof(tsp_flags) / sizeof(tsp_flags[0]);

    int mandatory_flags = 0;
    for (int i = 0; i < tsp_flag_size; i++) {
        if (tsp_flags[i]->mandatory) mandatory_flags++;
    }

    int parsed_mandatory_flags = 0;
    // Iterate through flags; stop at argc - 1 to ensure a following argument exists.
    for (int current_argv_parameter = 1; current_argv_parameter < argc; current_argv_parameter++) {
        for (int i = 0; i < tsp_flag_size; i++) {
            // Parse the flag with its associated value.
            const ParsingResult result = parse_flag(tsp_flags[i], argv, &current_argv_parameter);
            if (PARSE_SUCCESS == result) {
                if (tsp_flags[i]->mandatory) {
                    parsed_mandatory_flags++;
                }
                break;
            }

            if (i == tsp_flag_size - 1 && PARSE_NON_MATCHING_LABEL == result) {
                printf("The following argument has not been recognized: '%s'\n", argv[current_argv_parameter]);
            }

        }
    }

    if (parsed_mandatory_flags != mandatory_flags) {
        PARSING_ABORT(PARSE_MISSING_MANDATORY_FLAG);
    }
}
