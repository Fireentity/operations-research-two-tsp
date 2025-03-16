#ifndef FLAG_H
#define FLAG_H

#include <stdbool.h>
#include <parsing_result.h>
#include <parsing_util.h>
#include <stdint-gcc.h>

/** Forward declaration of FlagState. */
typedef struct FlagState FlagState;

/** Forward declaration of Flag. */
typedef struct Flag Flag;

/**
 * @brief Represents a flag with its state and operations.
 */
struct Flag {
    FlagState *state; /**< Pointer to the flag's state. */

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
 * @return Pointer to the initialized flag.
 */
const Flag *init_flag(const char *label,
                      unsigned int number_of_params,
                      ParsingResult (*param_function)(CmdOptions *cmd_options, const char **arg),
                      bool mandatory);

/**
 * @brief Computes a 64-bit hash for a flag.
 *
 * @param item Pointer to the flag.
 * @param seed0 First seed value.
 * @param seed1 Second seed value.
 * @return 64-bit hash value.
 */
uint64_t hash(const void *item, uint64_t seed0, uint64_t seed1);

/**
 * @brief Compares two flags.
 *
 * @param a Pointer to the first flag.
 * @param b Pointer to the second flag.
 * @param udata User data for comparison.
 * @return Comparison result.
 */
int compare(const void *a, const void *b, void *udata);

/**
 * @brief Frees memory allocated for a flag.
 *
 * @param item Pointer to the flag to free.
 */
void free_flag(void *item);

#endif //FLAG_H
