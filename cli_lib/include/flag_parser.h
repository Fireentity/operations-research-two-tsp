#ifndef FLAG_PARSER_H
#define FLAG_PARSER_H

#include "flag.h"

/**
 * @brief Opaque type representing the flag parser.
 */
typedef struct FlagParser FlagParser;

/**
 * @brief Opaque type representing a parser node.
 */
typedef struct ParserNode ParserNode;

/**
 * @brief Initializes the flag parser with the provided flags.
 *
 * @param flags The array of flags.
 * @return Pointer to the initialized FlagParser.
 */
FlagParser *init_flag_parser(struct FlagsArray flags);

/**
 * @brief Parses command-line arguments using the provided flag parser.
 *
 * @param cmd_options Pointer to the structure where parsed options will be stored.
 * @param parser The flag parser to use.
 * @param labels Null-terminated array of command-line arguments.
 * @param skip_unknowns
 * @return ParsingResult indicating success or the type of parsing error.
 */
const ParsingResult* parse_flags_with_parser(CmdOptions *cmd_options, const FlagParser *parser, const char **labels, bool skip_unknowns);

/**
 * @brief Frees the memory allocated for a FlagParser.
 *
 * This function releases all resources allocated for the given FlagParser,
 * including its internal data structures (e.g., the hashmap of ParserNodes).
 * Note: The externally provided flags are not freed by this function.
 *
 * @param parser Pointer to the FlagParser to free.
 */
void free_flag_parser(FlagParser *parser);

#endif //FLAG_PARSER_H
