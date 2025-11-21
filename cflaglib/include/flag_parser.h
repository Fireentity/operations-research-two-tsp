#ifndef FLAG_PARSER_H
#define FLAG_PARSER_H

#include <stdbool.h>
#include <stddef.h> // For size_t

#include "parsing_result.h"

/**
 * @brief A parsing context.
 * This is an "opaque type." The user can only create and use a
 * pointer to it. Its contents are defined in flag_parser.c
 * and are private to the library.
 */
typedef struct FlagParser FlagParser;

/**
 * @brief Flag properties (used as a bitmask).
 */
#define FLAG_MANDATORY 0x01
#define FLAG_OPTIONAL  0x00


/**
 * @brief Creates a new parsing context.
 *
 * @param options_struct A pointer to the user's CmdOptions struct
 * that will be populated.
 * @return A pointer to the new FlagParser, or NULL on error.
 */
FlagParser *flag_parser_new(void *options_struct);

/**
 * @brief Frees all memory associated with the FlagParser.
 */
void flag_parser_free(FlagParser *parser);

/**
 * @brief Registers a BOOLEAN (switch) flag.
 * If present, sets the boolean to 'true'.
 *
 * @param parser The parser context.
 * @param name The long name (e.g., "--help").
 * @param short_name The short name (e.g., "-h", or NULL).
 * @param description The description for --help text.
 * @param destination A pointer to the bool variable (bool*).
 * @param properties A bitmask (e.g., FLAG_OPTIONAL).
 */
void flag_parser_add_bool(FlagParser *parser,
                          const char *name,
                          const char *short_name,
                          const char *description,
                          bool *destination,
                          int properties);

/**
 * @brief Registers an INTEGER flag.
 * Expects one value (e.g., "--nodes 100").
 *
 * @param parser The parser context.
 * @param name The long name (e.g., "--help").
 * @param short_name The short name (e.g., "-h", or NULL).
 * @param description The description for --help text.
 * @param destination A pointer to the int variable (int*).
 * @param properties A bitmask (e.g., FLAG_OPTIONAL).
 */
void flag_parser_add_int(FlagParser *parser,
                         const char *name,
                         const char *short_name,
                         const char *description,
                         int *destination,
                         int properties);

/**
 * @brief Registers an UNSIGNED INT flag.
 *
 * @param parser The parser context.
 * @param name The long name (e.g., "--help").
 * @param short_name The short name (e.g., "-h", or NULL).
 * @param description The description for --help text.
 * @param destination A pointer to the int variable (int*).
 * @param destination A pointer to the unsigned int variable (unsigned int*).
 * @param properties A bitmask (e.g., FLAG_OPTIONAL).
 */
void flag_parser_add_uint(FlagParser *parser,
                          const char *name,
                          const char *short_name,
                          const char *description,
                          unsigned int *destination,
                          int properties);

/**
 * @brief Registers a FLOAT flag.
 *
 * @param parser The parser context.
 * @param name The long name (e.g., "--help").
 * @param short_name The short name (e.g., "-h", or NULL).
 * @param description The description for --help text.
 * @param destination A pointer to the float variable (float*).
 * @param properties A bitmask (e.g., FLAG_OPTIONAL).
 */
void flag_parser_add_float(FlagParser *parser,
                           const char *name,
                           const char *short_name,
                           const char *description,
                           float *destination,
                           int properties);

/**
 * @brief Registers a FLOAT flag (that must be >= 0).
 *
 * @param parser The parser context.
 * @param name The long name (e.g., "--help").
 * @param short_name The short name (e.g., "-h", or NULL).
 * @param description The description for --help text.
 * @param destination A pointer to the float variable (float*).
 * @param properties A bitmask (e.g., FLAG_OPTIONAL).
 */
void flag_parser_add_ufloat(FlagParser *parser,
                            const char *name,
                            const char *short_name,
                            const char *description,
                            float *destination,
                            int properties);

/**
 * @brief Registers a STRING (const char*) flag.
 *
 * @param parser The parser context.
 * @param name The long name (e.g., "--help").
 * @param short_name The short name (e.g., "-h", or NULL).
 * @param description The description for --help text.
 * @param destination A pointer to the string variable (const char**).
 * @param properties A bitmask (e.g., FLAG_OPTIONAL).
 */
void flag_parser_add_string(FlagParser *parser,
                            const char *name,
                            const char *short_name,
                            const char *description,
                            const char **destination,
                            int properties);

/**
 * @brief Registers an owned (i.e. can be freed) STRING (const char*) flag.
 *
 * @param parser The parser context.
 * @param name The long name (e.g., "--help").
 * @param short_name The short name (e.g., "-h", or NULL).
 * @param description The description for --help text.
 * @param destination A pointer to the string variable (const char**).
 * @param properties A bitmask (e.g., FLAG_OPTIONAL).
 */
void flag_parser_add_string_owned(FlagParser *parser,
                                  const char *name,
                                  const char *short_name,
                                  const char *description,
                                  char **destination,
                                  int properties);


/**
 * @brief Parses an array of string arguments.
 * Populates the user's struct and updates the internal 'visited_flags' state.
 *
 * @param parser The parser context.
 * @param argc The argument count.
 * @param argv The argument array.
 * @param skip_unknowns If 'true', ignores unknown flags.
 * If 'false', fails with PARSE_UNKNOWN_ARG.
 * @return A ParsingResult (e.g., SUCCESS, PARSE_UNKNOWN_ARG,
 * PARSE_WRONG_VALUE_TYPE, PARSE_USAGE_ERROR).
 */
const ParsingResult *flag_parser_parse(const FlagParser *parser,
                                       int argc,
                                       const char **argv,
                                       bool skip_unknowns);

/**
 * @brief Manually marks a flag as "visited".
 * Use this when loading from a config file or environment variables.
 *
 * @param parser The parser context.
 * @param flag_name The name of the flag to mark (e.g., "--nodes").
 * @return 'true' if the flag was found and marked, 'false' otherwise.
 */
bool flag_parser_mark_visited(const FlagParser *parser, const char *flag_name);

/**
 * @brief Search if a flag is marked as "visited".
 * Use this when loading from a config file or environment variables.
 *
 * @param parser The parser context.
 * @param flag_name The name of the flag to search for (e.g., "--nodes").
 * @return 'true' if the flag was found, and it's been visited, 'false' otherwise.
 */
bool flag_parser_is_visited(const FlagParser *parser, const char *flag_name);

/**
 * @brief Checks the final validation state.
 * Verifies that all flags marked as FLAG_MANDATORY have been
 * "visited" (either by argv or mark_visited).
 *
 * @return SUCCESS or PARSE_MISSING_MANDATORY_FLAG.
 */
const ParsingResult *flag_parser_validate(const FlagParser *parser);

/**
 * @brief Prints a well-formatted help message.
 * Prints to stdout, using the registered 'description' fields.
 */
void flag_parser_print_help(const FlagParser *parser);


#endif // FLAG_PARSER_H