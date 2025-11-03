#ifndef PARSING_RESULT_H
#define PARSING_RESULT_H

/**
 * @brief An enum of all possible parsing outcomes.
 */
typedef enum {
    PARSE_SUCCESS,
    PARSE_UNKNOWN_ARG,
    PARSE_MISSING_VALUE,
    PARSE_WRONG_VALUE_TYPE,
    PARSE_USAGE_ERROR, // e.g., repeating a mandatory flag
    PARSE_MISSING_MANDATORY_FLAG
} ParsingResultState;

/**
 * @brief A struct to hold the result of a parsing operation.
 * Functions will return a pointer to a constant instance of this struct.
 */
typedef struct {
    const ParsingResultState state;
    const char* error_message;
} ParsingResult;

/*
 * GLOBAL RESULT CONSTANTS
 *
 * These are *declared* here (with extern) and will be *defined*
 * in parsing_result.c
 */

/**
 * @brief Returned on successful parsing.
 */
extern const ParsingResult* SUCCESS;

/**
 * @brief Returned when an argument is not recognized (e.g., "--foo").
 */
extern const ParsingResult* UNKNOWN_ARG;

/**
 * @brief Returned when a flag expects a value but none is given
 * (e.g., "--nodes" followed by nothing).
 */
extern const ParsingResult* MISSING_VALUE;

/**
 * @brief Returned when a flag gets a value of the wrong type
 * (e.g., "--nodes abc").
 */
extern const ParsingResult* WRONG_VALUE_TYPE;

/**
 * @brief Returned for invalid usage (e.g., repeating a mandatory flag).
 */
extern const ParsingResult* USAGE_ERROR;

/**
 * @brief Returned by the final validation step if a mandatory flag was not found.
 */
extern const ParsingResult* MISSING_MANDATORY_FLAG;


#endif // PARSING_RESULT_H