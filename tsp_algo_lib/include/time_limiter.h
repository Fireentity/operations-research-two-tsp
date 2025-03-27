#ifndef TIME_LIMITER_H
#define TIME_LIMITER_H

#include <stdbool.h>

typedef struct TimeLimiter TimeLimiter;

typedef struct TimeLimiterState TimeLimiterState;

struct TimeLimiter
{
    TimeLimiterState* const state;
    bool (*const is_time_over)(const TimeLimiter*);
    void (*const start)(const TimeLimiter*);
    void (*const free)(const TimeLimiter*);
};

TimeLimiter *init_time_limiter(double time_limit_seconds);

#endif //TIME_LIMITER_H
