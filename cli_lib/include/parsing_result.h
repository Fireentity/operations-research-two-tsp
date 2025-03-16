#ifndef PARSING_RESULT_H
#define PARSING_RESULT_H

/**
 * @file parsing_result.h
 * @brief Contains the ParsingResult enumeration.
 */

/**
 * @brief Enumeration of possible outcomes for parsing operations.
 */
typedef enum
{
    PARSE_SUCCESS = 0,                  /**< Parsing operation was successful. */
    PARSE_NON_MATCHING_LABEL = 1,       /**< The provided label did not match. */
    PARSE_UNKNOWN_ARG = 2,              /**< An unknown argument was encountered. */
    PARSE_USAGE_ERROR = 3,              /**< There was a usage error. */
    PARSE_WRONG_VALUE_TYPE = 4,         /**< The provided value type is incorrect. */
    PARSE_MISSING_VALUE = 5,            /**< A required value is missing. */
    PARSE_MISSING_MANDATORY_FLAG = 6    /**< A mandatory flag is missing. */
} ParsingResult;

#endif //PARSING_RESULT_H
