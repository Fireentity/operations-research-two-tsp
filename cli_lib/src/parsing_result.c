#include "parsing_result.h"

const char *parsing_result_to_string(ParsingResult result) {
    switch (result) {
    case PARSE_SUCCESS: return "PARSE_SUCCESS";
    case PARSE_NON_MATCHING_LABEL: return "PARSE_NON_MATCHING_LABEL";
    case PARSE_UNKNOWN_ARG: return "PARSE_UNKNOWN_ARG";
    case PARSE_USAGE_ERROR: return "PARSE_USAGE_ERROR";
    case PARSE_WRONG_VALUE_TYPE: return "PARSE_WRONG_VALUE_TYPE";
    case PARSE_MISSING_VALUE: return "PARSE_MISSING_VALUE";
    case PARSE_MISSING_MANDATORY_FLAG: return "PARSE_MISSING_MANDATORY_FLAG";
    default: return "UNKNOWN_PARSING_RESULT";
    }
}