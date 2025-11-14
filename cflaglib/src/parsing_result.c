#include <stddef.h>
#include "parsing_result.h"

/*
 * Here we define the actual constant objects.
 * The 'static' keyword keeps the structs private to this file.
 * The 'const' pointers (e.g., SUCCESS) are the public-facing API,
 * pointing to the private static structs.
 */

static const ParsingResult SUCCESS_RESULT = {
    .state = PARSE_SUCCESS,
    .error_message = NULL
};
const ParsingResult *SUCCESS = &SUCCESS_RESULT;

static const ParsingResult HELP_RESULT = {
    .state = PARSE_HELP,
    .error_message = NULL
};
const ParsingResult *HELP = &HELP_RESULT;

static const ParsingResult UNKNOWN_ARG_RESULT = {
    .state = PARSE_UNKNOWN_ARG,
    .error_message = "Error: Unknown argument provided."
};
const ParsingResult *UNKNOWN_ARG = &UNKNOWN_ARG_RESULT;

static const ParsingResult MISSING_VALUE_RESULT = {
    .state = PARSE_MISSING_VALUE,
    .error_message = "Error: Flag is missing its required value."
};
const ParsingResult *MISSING_VALUE = &MISSING_VALUE_RESULT;

static const ParsingResult WRONG_VALUE_TYPE_RESULT = {
    .state = PARSE_WRONG_VALUE_TYPE,
    .error_message = "Error: Flag received a value of the wrong type."
};
const ParsingResult *WRONG_VALUE_TYPE = &WRONG_VALUE_TYPE_RESULT;

static const ParsingResult USAGE_ERROR_RESULT = {
    .state = PARSE_USAGE_ERROR,
    .error_message = "Error: Invalid flag usage (e.g., repeating a flag)."
};
const ParsingResult *USAGE_ERROR = &USAGE_ERROR_RESULT;

static const ParsingResult MISSING_MANDATORY_FLAG_RESULT = {
    .state = PARSE_MISSING_MANDATORY_FLAG,
    .error_message = "Error: A required mandatory flag was not provided."
};
const ParsingResult *MISSING_MANDATORY_FLAG = &MISSING_MANDATORY_FLAG_RESULT;

static const ParsingResult INTERNAL_ERROR_RESULT = {
    .state = PARSE_INTERNAL_ERROR,
    .error_message = "Error: An internal error has occurred."
};
const ParsingResult *INTERNAL_ERROR = &INTERNAL_ERROR_RESULT;
