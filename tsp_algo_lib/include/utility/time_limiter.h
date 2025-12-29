#ifndef TIME_LIMITER_H
#define TIME_LIMITER_H

#include <stdbool.h>

/**
 * @brief Lightweight time limiter (value-type).
 * Tracks elapsed time against a fixed limit.
 */
typedef struct {
    double start_time;
    double limit_seconds;
} TimeLimiter;

/**
 * @brief Creates a time limiter with the given max duration.
 */
TimeLimiter time_limiter_create(double limit_seconds);

/**
 * @brief Starts or restarts the timer from the current time.
 */
void time_limiter_start(TimeLimiter *limiter);

/**
 * @brief Checks whether the time limit has been exceeded.
 */
bool time_limiter_is_over(const TimeLimiter *limiter);

/**
 * @brief Returns the remaining time in seconds.
 * Returns 0.0 if the time limit has already been exceeded.
 */
double time_limiter_get_remaining(const TimeLimiter *limiter);
#endif // TIME_LIMITER_H
