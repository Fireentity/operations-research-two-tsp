#include "time_limiter.h"

#include <chrono.h>
#include <stdbool.h>

#include "c_util.h"

struct TimeLimiterState
{
    double start_seconds;
    const double time_limit_seconds;
};

bool is_time_over(const TimeLimiter* self)
{
    return second() - self->state->start_seconds > self->state->time_limit_seconds;
}

void start(const TimeLimiter* const self)
{
    self->state->start_seconds = second();
}

void free_this(const TimeLimiter* self)
{
    free(self->state);
    free((void*)self);
}

TimeLimiter *init_time_limiter(const double time_limit_seconds)
{
    const TimeLimiterState state = {
        .start_seconds = 0,
        .time_limit_seconds = time_limit_seconds,
    };
    const TimeLimiter time_limiter = {
        .is_time_over = is_time_over,
        .start = start,
        .free = free_this,
        .state = malloc_from_stack(&state, sizeof(state)),
    };
    return malloc_from_stack(&time_limiter, sizeof(time_limiter));
}
