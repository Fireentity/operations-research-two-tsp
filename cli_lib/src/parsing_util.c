#include <c_util.h>
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

DEFINE_COUNT_IF(Flag*, flag);

bool is_mandatory(Flag** tsp_flags, int i)
{
    return tsp_flags[i]->is_mandatory(tsp_flags[i]);
}



// Function to parse the command-line arguments using flags.
ParsingResult parse_flags(CmdOptions* cmd_options,
                          const Flag** tsp_flags,
                          const int number_of_flags,
                          const int argc,
                          const char** argv)
{
    const int mandatory_flags = count_flag_if(tsp_flags, number_of_flags, is_mandatory);
    const Flag* local_flags[number_of_flags];
    memcpy(local_flags, tsp_flags, number_of_flags * sizeof(*tsp_flags));

    int parsed_mandatory_flags = 0;
    int parsed_flags_count = 0;

    for (unsigned int current_param = 1; current_param < argc-1; current_param++)
    {
        ParsingResult result;
        const Flag* current_flag;
        int i = parsed_flags_count;
        do
        {
            current_flag = local_flags[i];
            if (current_param + current_flag->get_number_of_params(current_flag) >= argc)
                return PARSE_MISSING_VALUE;
            result = current_flag->parse(current_flag, cmd_options, argv, &current_param);
            i++;
        }
        while (result != PARSE_SUCCESS && i < number_of_flags);

        if (result == PARSE_SUCCESS && current_flag->is_mandatory(current_flag))
        {
            parsed_mandatory_flags++;
        }
        if (result == PARSE_SUCCESS)
        {
            //swap_int(local_flags[parsed_flags_count], local_flags[i - 1]);
            parsed_flags_count++;
        }
        else
        {
            return PARSE_UNKNOWN_ARG;
        }
    }

    return parsed_mandatory_flags != mandatory_flags ? PARSE_MISSING_MANDATORY_FLAG : PARSE_SUCCESS;
}
