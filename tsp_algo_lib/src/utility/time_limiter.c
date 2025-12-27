#include "time_limiter.h"
#include "chrono.h"
#include "logger.h"

TimeLimiter time_limiter_create(const double limit_seconds) {
    return (TimeLimiter){
        .start_time = 0.0,
        .limit_seconds = limit_seconds
    };
}

void time_limiter_start(TimeLimiter *limiter) {
    limiter->start_time = second();
    if_verbose(VERBOSE_DEBUG,
               "TimeLimiter: start=%.4f, limit=%.2f\n",
               limiter->start_time,
               limiter->limit_seconds);
}

bool time_limiter_is_over(const TimeLimiter *limiter) {
    const double elapsed = second() - limiter->start_time;
    return elapsed > limiter->limit_seconds;
}

double time_limiter_get_remaining(const TimeLimiter *limiter) {
    const double now = second();
    const double elapsed = now - limiter->start_time;
    const double remaining = limiter->limit_seconds - elapsed;

    return remaining > 0.0 ? remaining : 0.0;
}
