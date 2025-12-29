#include "logger.h"

/**
 * @brief The global verbosity level.
 * It is 'static', so it is visible ONLY within this .c file.
 */
static unsigned int g_verbosity_level = VERBOSE_ALL;


void logger_set_verbosity(const unsigned int level) {
    g_verbosity_level = level;
}

unsigned int logger_get_verbosity(void) {
    return g_verbosity_level;
}
