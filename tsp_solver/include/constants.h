#ifndef CONSTANTS_H
#define CONSTANTS_H

/**
 * @brief Verbosity level for logging.
 */
#define VERBOSITY 0

/**
 * @brief Padding used for plots.
 */
#define PLOT_PADDING 2

#define EPSILON 1e-10

#define WITH_MUTEX(mutex, code) do { \
pthread_mutex_lock(mutex);     \
code;                          \
pthread_mutex_unlock(mutex);   \
} while(0)
#endif //CONSTANTS_H
