#include "time_limiter.h"

#include <chrono.h>
#include <stdbool.h>
#include <stdlib.h> // For free

#include "c_util.h"
#include "logger.h" // For if_verbose

struct TimeLimiterState {
    double start_seconds;
    const double time_limit_seconds;
};

bool is_time_over(const TimeLimiter* self) {
    return second() - self->state->start_seconds > self->state->time_limit_seconds;
}

void start(const TimeLimiter* const self) {
    if_verbose(VERBOSE_DEBUG, "    TimeLimiter: Timer started (limit: %.2fs).\n", self->state->time_limit_seconds);
    self->state->start_seconds = second();
}

void free_this(const TimeLimiter* self) {
    if (!self) return;
    if_verbose(VERBOSE_DEBUG, "    TimeLimiter: Freeing timer.\n");
    if (self->state) {
        free(self->state);
    }
    free((void*)self);
}

TimeLimiter* init_time_limiter(const float time_limit_seconds) {
    if_verbose(VERBOSE_DEBUG, "    TimeLimiter: Initializing with limit %.2fs.\n", time_limit_seconds);
    const TimeLimiterState state = {
        .start_seconds = 0,
        .time_limit_seconds = time_limit_seconds,
    };
    const TimeLimiter time_limiter = {
        .is_time_over = is_time_over,
        .start = start,
        .free = free_this,
        .state = memdup(&state, sizeof(state)),
    };
    return memdup(&time_limiter, sizeof(time_limiter));
}
