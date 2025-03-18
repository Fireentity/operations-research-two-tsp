#ifndef PARSING_UTIL_H
#define PARSING_UTIL_H

#include <stdbool.h>
#include <parsing_result.h>


/**
 * @brief Parses an unsigned integer from a string.
 *
 * @param arg String containing the unsigned integer.
 * @param parsed Pointer to store the parsed unsigned integer.
 * @return ParsingResult outcome of the parsing.
 */
ParsingResult parse_unsigned_int(const char* arg, unsigned int* parsed);

/**
 * @brief Parses an integer from a string.
 *
 * @param arg String containing the integer.
 * @param parsed Pointer to store the parsed integer.
 * @return ParsingResult outcome of the parsing.
 */
ParsingResult parse_int(const char* arg, int* parsed);

/**
 * @brief Parses a floating point number from a string.
 *
 * @param arg String containing the float.
 * @param parsed Pointer to store the parsed float.
 * @return ParsingResult outcome of the parsing.
 */
ParsingResult parse_float(const char* arg, float* parsed);

/**
 * @brief Parses a boolean value from a string.
 *
 * @param arg String containing the boolean.
 * @param parsed Pointer to store the parsed boolean.
 * @return ParsingResult outcome of the parsing.
 */
ParsingResult parse_bool(const char* arg, bool* parsed);

#endif //PARSING_UTIL_H
