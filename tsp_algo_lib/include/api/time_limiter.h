#ifndef TIME_LIMITER_H
#define TIME_LIMITER_H

#include <stdbool.h>

/**
 * @brief Opaque structure for tracking execution time limits.
 */
typedef struct TimeLimiter TimeLimiter;

/**
 * @brief Creates a new TimeLimiter with the specified duration.
 * @param time_limit_seconds The maximum allowed duration in seconds.
 * @return Pointer to the new TimeLimiter.
 */
TimeLimiter *time_limiter_create(double time_limit_seconds);

/**
 * @brief Frees the memory associated with the TimeLimiter.
 */
void time_limiter_destroy(TimeLimiter *limiter);

/**
 * @brief Starts (or restarts) the timer from the current moment.
 */
void time_limiter_start(TimeLimiter *limiter);

/**
 * @brief Checks if the elapsed time has exceeded the limit.
 * @return true if time is over, false otherwise.
 */
bool time_limiter_is_over(const TimeLimiter *limiter);

#endif //TIME_LIMITER_H
