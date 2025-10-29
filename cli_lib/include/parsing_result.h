#ifndef PARSING_RESULT_H
#define PARSING_RESULT_H

/**
 * @brief Enumeration of possible outcomes for parsing operations.
 */
typedef enum {
    PARSE_SUCCESS = 0,
    PARSE_NON_MATCHING_LABEL = 1,
    PARSE_UNKNOWN_ARG = 2,
    PARSE_USAGE_ERROR = 3,
    PARSE_WRONG_VALUE_TYPE = 4,
    PARSE_MISSING_VALUE = 5,
    PARSE_MISSING_MANDATORY_FLAG = 6
} ParsingResultState;

typedef struct ParsingResult ParsingResult;

struct ParsingResult {
    ParsingResultState state;
    const char *const error_message;

    const ParsingResult *(*const of)(const ParsingResult *self, ...);
};


/**
 * @brief Pre‐built, immutable parsing results.
 *
 * Each of these lives for the lifetime of the program.
 */
extern const ParsingResult* SUCCESS;
extern const ParsingResult* NON_MATCHING_LABEL;
extern const ParsingResult* UNKNOWN_ARG;
extern const ParsingResult* USAGE_ERROR;
extern const ParsingResult* WRONG_VALUE_TYPE;
extern const ParsingResult* MISSING_VALUE;
extern const ParsingResult* MISSING_MANDATORY_FLAG;

#endif /* PARSING_RESULT_H */
