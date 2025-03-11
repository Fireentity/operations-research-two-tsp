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

    const Flag* tsp_flags[] = {
        init_flag("--nodes", 1, set_nodes, true),
        init_flag("--seed", 1, set_seed, false),
        init_flag("--x-square", 1, set_x_square, true),
        init_flag("--y-square", 1, set_y_square, true),
        init_flag("--square-side", 1, set_square_side, true),
        init_flag("--seconds", 1, set_time_limit, false),
        init_flag("--help", 0, set_help, false)
    };
    //TODO free di sta roba

    const int tsp_flag_size = sizeof(tsp_flags) / sizeof(tsp_flags[0]);

    int mandatory_flags = 0;
    //Counts the mandatory flags
    for (int i = 0; i < tsp_flag_size; i++) {
        if (tsp_flags[i]->mandatory) mandatory_flags++;
    }

    int parsed_mandatory_flags = 0;
    // Iterate through flags; stop at argc - 1 to ensure a following argument exists.
    for (int current_argv_parameter = 1; current_argv_parameter < argc; current_argv_parameter++) {
        for (int i = 0; i < tsp_flag_size; i++) {
            // Parse the flag with its associated value.
            const ParsingResult result = parse(tsp_flags[i], argv, &current_argv_parameter);

            //If the flag is parsed then break and parse the next flag
            if (PARSE_SUCCESS == result) {
                // Check if the current flag is mandatory; if so, increment the count of parsed mandatory flags
                if (tsp_flags[i]->mandatory) {
                    parsed_mandatory_flags++;
                }
                break;
            }

            // If this is the last flag and the argument did not match any flag, print an error message.
            if (i == tsp_flag_size - 1 && PARSE_NON_MATCHING_LABEL == result) {
                printf("Argument not recognized: '%s'\n", argv[current_argv_parameter]);
            }

        }
    }

    if (parsed_mandatory_flags != mandatory_flags) {
        PARSING_ABORT(PARSE_MISSING_MANDATORY_FLAG);
    }
}

