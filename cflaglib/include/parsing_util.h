#ifndef PARSING_UTIL_H
#define PARSING_UTIL_H
#include "parsing_result.h"

/**
 * @brief Safely parses a string into an integer.
 * This function handles overflow, underflow, and invalid characters.
 *
 * @param s The input string to parse (e.g., "123" or "abc").
 * @param dest A pointer to the integer to write the result to.
 * @return SUCCESS on success, or WRONG_VALUE_TYPE on failure.
 */
const ParsingResult* parse_int(const char* s, int* dest);

/**
 * @brief Safely parses a string into an unsigned integer.
 * This function handles overflow, negative numbers, and invalid characters.
 *
 * @param s The input string.
 * @param dest A pointer to the unsigned int to write the result to.
 * @return SUCCESS on success, or WRONG_VALUE_TYPE on failure.
 */
const ParsingResult* parse_uint(const char* s, unsigned int* dest);

/**
 * @brief Safely parses a string into a float.
 * This function handles overflow, underflow, and invalid characters.
 *
 * @param s The input string (e.g., "123.45").
 * @param dest A pointer to the float to write the result to.
 * @return SUCCESS on success, or WRONG_VALUE_TYPE on failure.
 */
const ParsingResult* parse_float(const char* s, float* dest);

/**
 * @brief Safely parses a string into an unsigned (non-negative) float.
 * This function reuses parse_float and adds a check for negative values.
 *
 * @param s The input string (e.g., "123.45").
 * @param dest A pointer to the float to write the result to.
 * @return SUCCESS on success, or WRONG_VALUE_TYPE on failure.
 */
const ParsingResult* parse_ufloat(const char* s, float* dest);


#endif // PARSING_UTIL_H
