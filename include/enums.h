#ifndef ENUMS_H
#define ENUMS_H

typedef enum
{
    PARSE_SUCCESS,
    PARSE_NON_MATCHING_LABEL,
    PARSE_UNKNOWN_ARG,
    PARSE_USAGE_ERROR,
    PARSE_WRONG_VALUE_TYPE,
    PARSE_MISSING_VALUE,
    PARSE_MISSING_MANDATORY_FLAG
} ParsingResult;

typedef enum
{
    DUPLICATED_ENTRY,
    UNINITIALIZED_ENTRY,
    NON_MATCHING_COST,
    FEASIBLE,
} FeasibilityResult;

const char* parsing_result_to_string(ParsingResult result);

const char* feasibility_result_to_string(FeasibilityResult result);

#define FEASIBILITY_ABORT(result) do { \
    printf("Solution in not feasible: %s\n", feasibility_result_to_string(feasibility_result)); \
    exit(EXIT_FAILURE); \
} while(0)

#endif // ENUMS_H
