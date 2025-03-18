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
 * @param argv Null-terminated array of command-line arguments.
 * @return ParsingResult indicating success or the type of parsing error.
 */
ParsingResult parse_flags_with_parser(CmdOptions *cmd_options, const FlagParser *parser, const char **argv);

#endif //FLAG_PARSER_H
