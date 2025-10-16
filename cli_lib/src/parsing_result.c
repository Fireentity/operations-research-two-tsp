#include "parsing_result.h"
#include "c_util.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define UNKNOWN_ARG_MESSAGE                   "Argument not recognized %s.\n\nUsage:\n"
#define PARSE_USAGE_ERROR_MESSAGE             "Usage error occurred. Please check the provided arguments.\n\nUsage:\n"
#define PARSE_WRONG_VALUE_TYPE_MESSAGE        "Wrong value type for the argument.\n\nUsage:\n"
#define PARSE_MISSING_VALUE_MESSAGE           "Missing value for the argument.\n\nUsage:\n"
#define PARSE_MISSING_MANDATORY_FLAG_MESSAGE  "Missing mandatory argument.\n\nUsage:\n"
#define INTERNAL_ERROR_MESSAGE                "Internal error – this should not appear. Report it to the developer."

static const ParsingResult *of(const ParsingResult *self, ...) {
    va_list args;
    va_start(args, self);
    char *formatted = vstr_format(self->error_message, args);
    va_end(args);

    const ParsingResult tmp = {
        .state = self->state,
        .error_message = formatted,
        .of = of
    };
    return memdup(&tmp, sizeof(tmp));
}

const ParsingResult SUCCESS_RESULT = {
    .state = PARSE_SUCCESS,
    .error_message = "Parsing operation was successful.\n",
    .of = of
};

const ParsingResult NON_MATCHING_LABEL_RESULT = {
    .state = PARSE_NON_MATCHING_LABEL,
    .error_message = "The provided label did not match.\n",
    .of = of
};

const ParsingResult UNKNOWN_ARG_RESULT = {
    .state = PARSE_UNKNOWN_ARG,
    .error_message = UNKNOWN_ARG_MESSAGE,
    .of = of
};

const ParsingResult USAGE_ERROR_RESULT = {
    .state = PARSE_USAGE_ERROR,
    .error_message = PARSE_USAGE_ERROR_MESSAGE,
    .of = of
};

const ParsingResult WRONG_VALUE_TYPE_RESULT = {
    .state = PARSE_WRONG_VALUE_TYPE,
    .error_message = PARSE_WRONG_VALUE_TYPE_MESSAGE,
    .of = of
};

const ParsingResult MISSING_VALUE_RESULT = {
    .state = PARSE_MISSING_VALUE,
    .error_message = PARSE_MISSING_VALUE_MESSAGE,
    .of = of
};

const ParsingResult MISSING_MANDATORY_FLAG_RESULT = {
    .state = PARSE_MISSING_MANDATORY_FLAG,
    .error_message = PARSE_MISSING_MANDATORY_FLAG_MESSAGE,
    .of = of
};

static const ParsingResult INTERNAL_ERROR_RESULT = {
    .state = PARSE_SUCCESS,  /* treated as an internal fallback */
    .error_message = INTERNAL_ERROR_MESSAGE,
    .of = of
};

/* Assign the extern pointers declared in parsing_result.h to the above instances */
const ParsingResult* SUCCESS               = &SUCCESS_RESULT;
const ParsingResult* NON_MATCHING_LABEL    = &NON_MATCHING_LABEL_RESULT;
const ParsingResult* UNKNOWN_ARG           = &UNKNOWN_ARG_RESULT;
const ParsingResult* USAGE_ERROR           = &USAGE_ERROR_RESULT;
const ParsingResult* WRONG_VALUE_TYPE      = &WRONG_VALUE_TYPE_RESULT;
const ParsingResult* MISSING_VALUE         = &MISSING_VALUE_RESULT;
const ParsingResult* MISSING_MANDATORY_FLAG = &MISSING_MANDATORY_FLAG_RESULT;
