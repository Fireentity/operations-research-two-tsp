#ifndef PARSING_RESULT_H
#define PARSING_RESULT_H

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

const char* parsing_result_to_string(ParsingResult result);


#endif //PARSING_RESULT_H
