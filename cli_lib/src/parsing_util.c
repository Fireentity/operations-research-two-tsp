#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <flag.h>
#include <parsing_util.h>

ParsingResult parse_unsigned_int(const char* arg, unsigned int* parsed)
{
    char* end;
    errno = 0;
    const int val = strtoul(arg, &end, 10);
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

// Function to parse the command-line arguments using flags.
ParsingResult parse_flags(CmdOptions* cmd_options,
                 const Flag** tsp_flags,
                 const int number_of_flags,
                 const int argc,
                 const char** argv)
{
    int mandatory_flags = 0;
    //Counts the mandatory flags
    for (int i = 0; i < number_of_flags; i++)
    {
        if (tsp_flags[i]->is_mandatory(tsp_flags[i])) mandatory_flags++;
    }

    int parsed_mandatory_flags = 0;
    // Iterate through flags; stop at argc - 1 to ensure a following argument exists.
    for (unsigned int current_argv_parameter = 1; current_argv_parameter < argc; current_argv_parameter++)
    {
        const Flag** current_flag = tsp_flags;
        ParsingResult result;
        do
        {
            result = (*current_flag)->parse(*current_flag, cmd_options, argv, &current_argv_parameter);
            current_flag++;
        } while (result != PARSE_SUCCESS && current_flag < tsp_flags + number_of_flags);
        current_flag--;

        if (result == PARSE_SUCCESS && (*current_flag)->is_mandatory(*current_flag))
        {
            parsed_mandatory_flags++;
        }

        if (result != PARSE_SUCCESS)
        {
            return PARSE_UNKNOWN_ARG;
        }
    }

    if (parsed_mandatory_flags != mandatory_flags)
    {
        return PARSE_MISSING_MANDATORY_FLAG;
    }

    return PARSE_SUCCESS;
}
