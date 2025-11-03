#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>

#define VERBOSE_NONE  0
#define VERBOSE_INFO  1
#define VERBOSE_DEBUG 2
#define VERBOSE_ALL   3

/**
 * @brief Sets the global verbosity level for the application.
 *
 * Call this once at startup from main().
 * @param level The verbosity level (e.g., VERBOSE_INFO).
 */
void logger_set_verbosity(unsigned int level);

/**
 * @brief Gets the current global verbosity level.
 *
 * @return The globally set verbosity level.
 */
unsigned int logger_get_verbosity(void);

#ifndef DISABLE_VERBOSE
/**
 * @brief Internal helper function for printing.
 * Do NOT call directly; use the if_verbose macro.
 */
static inline void logger_print(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

/**
 * @brief Conditionally prints to stdout based on the global verbosity level.
 *
 * Example:
 * if_verbose(VERBOSE_INFO, "Parsing complete.\n");
 * if_verbose(VERBOSE_DEBUG, "Found %d nodes.\n", 10);
 */
#define if_verbose(required_level, ...) \
do { \
if (logger_get_verbosity() >= (required_level)) { \
logger_print(__VA_ARGS__); \
} \
} while(0)

#else
// If disabled, the macro compiles to nothing.
#define if_verbose(required_level, ...) ((void)0)
#endif // DISABLE_VERBOSE

#endif // LOGGER_H
