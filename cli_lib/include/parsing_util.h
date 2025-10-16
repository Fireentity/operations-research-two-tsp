#ifndef PARSING_UTIL_H
#define PARSING_UTIL_H

#include <stdbool.h>
#include "parsing_result.h"


const ParsingResult* parse_string(const char* arg, const char** parsed) ;
/**
 * @brief Parses an unsigned integer from a string.
 *
 * @param arg String containing the unsigned integer.
 * @param parsed Pointer to store the parsed unsigned integer.
 * @return Pointer to a ParsingResult indicating success or error.
 */
const ParsingResult* parse_unsigned_int(const char* arg, unsigned int* parsed);

/**
 * @brief Parses a signed integer from a string.
 *
 * @param arg String containing the integer.
 * @param parsed Pointer to store the parsed integer.
 * @return Pointer to a ParsingResult indicating success or error.
 */
const ParsingResult* parse_int(const char* arg, int* parsed);

/**
 * @brief Parses a floating point number from a string.
 *
 * @param arg String containing the float.
 * @param parsed Pointer to store the parsed float.
 * @return Pointer to a ParsingResult indicating success or error.
 */
const ParsingResult* parse_float(const char* arg, float* parsed);

/**
 * @brief Parses a boolean value from a string.
 *
 * @param arg String containing the boolean.
 * @param parsed Pointer to store the parsed boolean.
 * @return Pointer to a ParsingResult indicating success or error.
 */
const ParsingResult* parse_bool(const char* arg, bool* parsed);

#endif //PARSING_UTIL_H
