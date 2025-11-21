#include "time_limiter.h"

#include <stdlib.h>
#include "chrono.h" // For second()
#include "c_util.h" // For check_alloc
#include "logger.h"

// Definition of the opaque struct (Flattened: data is direct)
struct TimeLimiter {
    double start_time;
    double limit_duration;
};

TimeLimiter* time_limiter_create(const double time_limit_seconds) {
    if_verbose(VERBOSE_DEBUG, "    TimeLimiter: Initializing with limit %.2fs.\n", time_limit_seconds);

    TimeLimiter* limiter = malloc(sizeof(TimeLimiter));
    check_alloc(limiter);

    limiter->start_time = 0.0;
    limiter->limit_duration = time_limit_seconds;

    return limiter;
}

void time_limiter_destroy(TimeLimiter* limiter) {
    if (!limiter) return;

    if_verbose(VERBOSE_DEBUG, "    TimeLimiter: Freeing timer.\n");
    free(limiter);
}

void time_limiter_start(TimeLimiter* limiter) {
    if (!limiter) return;

    limiter->start_time = second();
    if_verbose(VERBOSE_DEBUG, "    TimeLimiter: Timer started at %.4fs (limit: %.2fs).\n",
               limiter->start_time, limiter->limit_duration);
}

bool time_limiter_is_over(const TimeLimiter* limiter) {
    if (!limiter) return true; // Safer default

    double elapsed = second() - limiter->start_time;
    return elapsed > limiter->limit_duration;
}