#include "enums.h"

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

const char *feasibility_result_to_string(FeasibilityResult result) {
    switch (result) {
        case DUPLICATED_ENTRY: return "DUPLICATED_ENTRY";
        case UNINITIALIZED_ENTRY: return "UNINITIALIZED_ENTRY";
        case NON_MATCHING_COST: return "NON_MATCHING_COST";
        case FEASIBLE: return "FEASIBLE";
        default: return "UNKNOWN_FEASIBILITY_RESULT";
    }
}
