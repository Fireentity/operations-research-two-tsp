#ifndef FLAG_H
#define FLAG_H

#include <stdbool.h>
#include <parsing_result.h>

/** Forward declaration of FlagState. */
typedef struct FlagState FlagState;

/** Forward declaration of Flag. */
typedef struct Flag Flag;

/** Forward declaration of CmdOptions. */
typedef struct CmdOptions CmdOptions;

struct FlagsArray {
    const Flag** flags;
    const unsigned int number_of_flags;
};

/**
 * @brief Represents a flag with its state and operations.
 */
struct Flag {

    FlagState *state; /**< Pointer to the flag's state. */

    struct FlagsArray children;
    /**
     * @brief Parses the flag from command line arguments.
     *
     * @param flag Pointer to the flag instance.
     * @param cmd_options Pointer to command options.
     * @param argv Array of command line arguments.
     * @param index Current index in argv.
     * @return ParsingResult outcome of the parsing.
     */
    ParsingResult (*const parse)(const Flag *flag,
                                 CmdOptions *cmd_options,
                                 const char **argv,
                                 unsigned int *index);

    /**
     * @brief Checks if the flag is mandatory.
     *
     * @param self Pointer to the flag instance.
     * @return true if mandatory, false otherwise.
     */
    bool (*const is_mandatory)(const Flag *self);

    /**
     * @brief Returns the expected number of parameters for the flag.
     *
     * @param self Pointer to the flag instance.
     * @return Number of parameters.
     */
    unsigned int (*const get_number_of_params)(const Flag *self);

    /**
     * @brief Retrieves the label of the flag.
     *
     * @param self Pointer to the flag instance.
     * @return String label of the flag.
     */
    const char* (*const get_label)(const Flag *self);
};

/**
 * @brief Initializes a flag.
 *
 * @param label The flag's label.
 * @param number_of_params Expected number of parameters.
 * @param param_function Function pointer for parameter parsing.
 * @param mandatory Whether the flag is mandatory.
 * @param children
 * @return Pointer to the initialized flag.
 */
const Flag *init_flag(const char *label,
                      unsigned int number_of_params,
                      ParsingResult (*param_function)(CmdOptions *cmd_options, const char **arg),
                      bool mandatory, struct FlagsArray children);
void add_children(Flag *self, struct FlagsArray children);
#endif //FLAG_H
