#include "flag_parser.h"
#include "parsing_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#define INITIAL_CAPACITY 8

/**
 * @brief Internal enum to track the type of flag.
 */
typedef enum {
    FLAG_TYPE_BOOL,
    FLAG_TYPE_INT,
    FLAG_TYPE_UINT,
    FLAG_TYPE_DOUBLE,
    FLAG_TYPE_UDOUBLE,
    FLAG_TYPE_STRING,
    FLAG_TYPE_STRING_OWNED
} FlagType;

/**
 * @brief The internal definition of a flag.
 * This struct holds all information about a registered flag.
 */
typedef struct {
    char *name; // Long name, e.g., "--nodes" (heap-allocated)
    char *short_name; // Short name, e.g., "-n" (heap-allocated)
    char *description; // Help text (heap-allocated)
    FlagType type;
    int properties; // Bitmask (e.g., FLAG_MANDATORY)
    void *destination; // Pointer to the user's variable
} FlagDefinition;

/**
 * @brief The *real* definition of the FlagParser struct.
 * This is what the opaque FlagParser* pointer points to.
 */
struct FlagParser {
    void *user_options_struct; // Pointer to the user's CmdOptions

    FlagDefinition **definitions; // Dynamic array of pointers to definitions
    size_t definition_count;
    size_t definition_capacity;

    bool *visited_flags; // Dynamic array, parallel to 'definitions'
};

// PRIVATE FUNCTIONS
/**
 * @brief Frees the memory for a single FlagDefinition.
 */
static void free_flag_definition(FlagDefinition *def) {
    if (!def) return;
    free(def->name);
    free(def->short_name);
    free(def->description);
    free(def);
}

/**
 * @brief Finds a flag by its long or short name.
 * @return The index of the flag, or -1 if not found.
 */
static int find_flag_by_name(const FlagParser *parser, const char *name) {
    for (size_t i = 0; i < parser->definition_count; i++) {
        const FlagDefinition *def = parser->definitions[i];
        if (def->name && strcmp(name, def->name) == 0) {
            return (int) i;
        }
        if (def->short_name && strcmp(name, def->short_name) == 0) {
            return (int) i;
        }
    }
    return -1;
}

/**
 * @brief Adds a new, fully formed definition to the parser's arrays.
 * Handles dynamic array resizing (realloc).
 * @return true on success, false on memory allocation failure.
 */
static bool flag_parser_add_definition(FlagParser *parser, FlagDefinition *def) {
    // Check if we need to grow the arrays
    if (parser->definition_count >= parser->definition_capacity) {
        const size_t new_capacity = (parser->definition_capacity == 0)
                                        ? INITIAL_CAPACITY
                                        : parser->definition_capacity * 2;

        // Resize definitions array
        void *defs_ptr = realloc(parser->definitions, new_capacity * sizeof(FlagDefinition *));
        if (!defs_ptr) {
            free_flag_definition(def); // Clean up the passed-in def
            return false;
        }
        parser->definitions = defs_ptr;

        // Resize visited_flags array
        void *visited_ptr = realloc(parser->visited_flags, new_capacity * sizeof(bool));
        if (!visited_ptr) {
            // TODO handle this better
            free_flag_definition(def);
            return false;
        }
        parser->visited_flags = visited_ptr;

        parser->definition_capacity = new_capacity;
    }

    // Add the new definition and initialize its visited state
    parser->definitions[parser->definition_count] = def;
    parser->visited_flags[parser->definition_count] = false;
    parser->definition_count++;

    return true;
}

/**
 * @brief Internal helper to create and add a new definition.
 */
static void add_flag_internal(FlagParser *parser,
                              const char *name,
                              const char *short_name,
                              const char *description,
                              void *destination,
                              const int properties,
                              const FlagType type) {
    if (!parser || !name || !destination) {
        return;
    }

    FlagDefinition *def = calloc(1, sizeof(FlagDefinition));
    if (!def) {
        return;
    }

    // Copy strings to the heap so the user doesn't have to keep them
    def->name = strdup(name);
    def->short_name = short_name ? strdup(short_name) : NULL;
    def->description = description ? strdup(description) : NULL;

    def->destination = destination;
    def->properties = properties;
    def->type = type;

    if (!flag_parser_add_definition(parser, def)) {
        // Allocation failed inside the helper, it this case the free is done in the helper
        fprintf(stderr, "Failed to add flag definition. Out of memory.\n");
    }
}

//PUBLIC FUNCTIONS
FlagParser *flag_parser_new(void *options_struct) {
    FlagParser *parser = calloc(1, sizeof(FlagParser));
    if (!parser) {
        return NULL;
    }
    parser->user_options_struct = options_struct;
    // Arrays will be allocated on the first add
    return parser;
}

void flag_parser_free(FlagParser *parser) {
    if (!parser) return;

    // Free all the individual definitions, the arrays themselves and the parser
    for (size_t i = 0; i < parser->definition_count; i++) {
        free_flag_definition(parser->definitions[i]);
    }
    free(parser->definitions);
    free(parser->visited_flags);
    free(parser);
}

void flag_parser_add_bool(FlagParser *parser,
                          const char *name,
                          const char *short_name,
                          const char *description,
                          bool *destination,
                          const int properties) {
    add_flag_internal(parser, name, short_name, description, destination, properties, FLAG_TYPE_BOOL);
}

void flag_parser_add_int(FlagParser *parser,
                         const char *name,
                         const char *short_name,
                         const char *description,
                         int *destination,
                         const int properties) {
    add_flag_internal(parser, name, short_name, description, destination, properties, FLAG_TYPE_INT);
}

void flag_parser_add_uint(FlagParser *parser,
                          const char *name,
                          const char *short_name,
                          const char *description,
                          unsigned int *destination,
                          const int properties) {
    add_flag_internal(parser, name, short_name, description, destination, properties, FLAG_TYPE_UINT);
}

void flag_parser_add_double(FlagParser *parser,
                            const char *name,
                            const char *short_name,
                            const char *description,
                            double *destination,
                            const int properties) {
    add_flag_internal(parser, name, short_name, description, destination, properties, FLAG_TYPE_DOUBLE);
}

void flag_parser_add_udouble(FlagParser *parser,
                             const char *name,
                             const char *short_name,
                             const char *description,
                             double *destination,
                             const int properties) {
    add_flag_internal(parser, name, short_name, description, destination, properties, FLAG_TYPE_UDOUBLE);
}

void flag_parser_add_string(FlagParser *parser,
                            const char *name,
                            const char *short_name,
                            const char *description,
                            const char **destination,
                            const int properties) {
    add_flag_internal(parser, name, short_name, description, destination, properties, FLAG_TYPE_STRING);
}

void flag_parser_add_string_owned(FlagParser *parser,
                                  const char *name,
                                  const char *short_name,
                                  const char *description,
                                  char **destination,
                                  const int properties) {
    add_flag_internal(parser, name, short_name, description, destination, properties, FLAG_TYPE_STRING_OWNED);
}

const ParsingResult *flag_parser_parse(const FlagParser *parser,
                                       const int argc,
                                       const char **argv,
                                       const bool skip_unknowns) {
    // TODO make better: for instance the -- or - is too specific i think
    for (int i = 0; i < argc; i++) {
        const char *arg = argv[i];

        // Stop parsing at "--"
        if (strcmp(arg, "--") == 0) {
            break;
        }

        // We only parse flags (starting with '-')
        if (arg[0] != '-') {
            if (skip_unknowns) continue;
            return UNKNOWN_ARG; // Or handle positional args if extended
        }

        // Find the flag definition
        const int def_index = find_flag_by_name(parser, arg);
        if (def_index == -1) {
            if (skip_unknowns) continue;
            return UNKNOWN_ARG;
        }

        const FlagDefinition *def = parser->definitions[def_index];

        // Check for duplicate mandatory flag
        if (parser->visited_flags[def_index]) {
            return USAGE_ERROR;
        }

        parser->visited_flags[def_index] = true;

        // --- Handle flag based on type ---

        if (def->type == FLAG_TYPE_BOOL) {
            // Boolean flags are simple switches, they take no value
            *(bool *) def->destination = true;
            continue;
        }

        // All other types need a value
        if (i + 1 >= argc) {
            return MISSING_VALUE; // Ran out of arguments
        }

        const char *value_str = argv[i + 1];

        // Check if the next token is another flag, which means this one is missing its value
        if (value_str[0] == '-') {
            // Technically, "-123" is a valid number, so we must be careful
            // A simple check: if it's not a bool and the next token is
            // a *known* flag, it's a missing value.
            if (find_flag_by_name(parser, value_str) != -1) {
                return MISSING_VALUE;
            }
            // Otherwise, we assume it's a negative number and let the
            // conversion functions handle it (e.g., parse_int)
        }

        // Consume the value argument
        ++i;
        const ParsingResult *res = SUCCESS;

        switch (def->type) {
            case FLAG_TYPE_INT:
                res = parse_int(value_str, def->destination);
                break;
            case FLAG_TYPE_UINT:
                res = parse_uint(value_str, def->destination);
                break;
            case FLAG_TYPE_DOUBLE:
                res = parse_double(value_str, def->destination);
                break;
            case FLAG_TYPE_UDOUBLE:
                res = parse_udouble(value_str, def->destination);
                break;
            case FLAG_TYPE_STRING:
                res = parse_string(value_str, def->destination);
                break;
            case FLAG_TYPE_BOOL:
                // Should not happen, was handled above
                break;
            case FLAG_TYPE_STRING_OWNED:
                ;
                char **dest_ptr = def->destination;
                if (*dest_ptr != NULL) {
                    free(*dest_ptr);
                }
                res = parse_string(value_str, dest_ptr);
                break;
        }

        if (res != SUCCESS) {
            // Propagate the specific error (e.g., WRONG_VALUE_TYPE)
            return res;
        }
    }

    return SUCCESS;
}

bool flag_parser_mark_visited(const FlagParser *parser, const char *flag_name) {
    const int def_index = find_flag_by_name(parser, flag_name);
    if (def_index != -1) {
        parser->visited_flags[def_index] = true;
        return true;
    }
    return false;
}

void flag_parser_reset_visited(FlagParser *parser) {
    if (!parser || !parser->visited_flags) return;
    for (size_t i = 0; i < parser->definition_count; ++i)
        parser->visited_flags[i] = false;

    // Would be the same as the memset here but is more "future-proof"
    // memset(parser->visited_flags, 0, parser->definition_count * sizeof(bool));
}

bool flag_parser_is_visited(const FlagParser *parser, const char *flag_name) {
    const int def_index = find_flag_by_name(parser, flag_name);
    if (def_index != -1) {
        return parser->visited_flags[def_index];
    }
    return false;
}

const ParsingResult *flag_parser_validate(const FlagParser *parser) {
    for (size_t i = 0; i < parser->definition_count; i++) {
        const FlagDefinition *def = parser->definitions[i];

        // If mandatory but was never visited
        if ((def->properties & FLAG_MANDATORY) && !parser->visited_flags[i]) {
            // TODO return a more specific error with the flag name
            return MISSING_MANDATORY_FLAG;
        }
    }
    return SUCCESS;
}


/**
 * @brief Prints a formatted and aligned help message for the parser.
 *
 * This function prints a usage line and a list of options. It uses a
 * two-pass approach to calculate column widths, ensuring all descriptions
 * are perfectly aligned, even with different flag name lengths.
 * It also correctly handles multi-line descriptions (separated by '\n').
 *
 * @param parser The initialized flag parser containing the definitions.
 */
void flag_parser_print_help(const FlagParser *parser) {
    const int INDENT = 2; // Spaces before the flag (e.g., "  --help")
    const int PADDING = 2; // Spaces between the longest flag and its description

    // Find the maximum width of the flag column
    size_t max_flag_width = 0;
    for (size_t i = 0; i < parser->definition_count; i++) {
        const FlagDefinition *def = parser->definitions[i];
        size_t current_width = INDENT;

        if (def->short_name) {
            // e.g., "-s, " (length of short_name + 2 chars ", ")
            current_width += strlen(def->short_name) + 2;
        } else {
            // No short name; add padding to align with flags that do
            current_width += 4; // "    "
        }
        current_width += strlen(def->name); // e.g., "--verbosity"

        if (current_width > max_flag_width) {
            max_flag_width = current_width;
        }
    }

    // Calculate the exact column where the description text will start
    const int desc_start_col = (int) max_flag_width + PADDING;

    // --- Pass 2: Print the formatted output ---
    printf("Usage: [program] [options]\n\nOptions:\n");

    for (size_t i = 0; i < parser->definition_count; i++) {
        const FlagDefinition *def = parser->definitions[i];

        // Build the flag string (e.g., "  -s, --short" or "      --long")
        char flag_str[256]; // Buffer for the complete flag text
        char short_name_str[5] = "    "; // 4 spaces default alignment

        if (def->short_name) {
            snprintf(short_name_str, 5, "%s, ", def->short_name);
        }

        snprintf(flag_str, sizeof(flag_str), "  %s%s",
                 short_name_str,
                 def->name);

        // Print the flag column, right-padded to the calculated width.
        printf("%-*s", desc_start_col, flag_str);

        // Print the description, handling multi-line strings
        const char *desc = def->description ? def->description : "";
        const char *line_start = desc;
        const char *line_end;
        bool first_line = true;

        while ((line_end = strchr(line_start, '\n')) != NULL) {
            if (!first_line) {
                // Not the first line, so print indentation
                printf("%*s", desc_start_col, "");
            }
            // Print the line-up to (but not including) the '\n'
            printf("%.*s\n", (int) (line_end - line_start), line_start);
            line_start = line_end + 1; // Advance to the start of the next line
            first_line = false;
        }

        // Print the last line (or the only line if no '\n' was found)
        if (*line_start) {
            if (!first_line) {
                // Indent if this isn't the first line
                printf("%*s", desc_start_col, "");
            }
            printf("%s\n", line_start);
        } else if (first_line) {
            // If the description was empty (""), just print a final newline
            printf("\n");
        }
    }
}
