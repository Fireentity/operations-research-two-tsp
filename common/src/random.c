#include "random.h"
#include <stdlib.h>

// LCG constants
static const uint64_t MULTIPLIER = 6364136223846793005ULL;
static const uint64_t INCREMENT  = 1442695040888963407ULL;

// Hidden singleton instance
static RandomState g_global_rng = { .state = 0 };

// Internal helper
static uint32_t next_u32(RandomState *rng) {
    const uint64_t x = rng->state;
    rng->state = x * MULTIPLIER + INCREMENT;
    return (uint32_t)(rng->state >> 32);
}

// Thread-safe API
void random_init(RandomState *rng, const uint64_t seed) {
    rng->state = seed + INCREMENT;
    next_u32(rng); // Warm-up
}

int random_int(RandomState *rng, int min, int max) {
    if (min > max) { const int t = min; min = max; max = t; }
    const uint32_t range = (uint32_t)(max - min + 1);
    return min + (int)(next_u32(rng) % range);
}

double random_double(RandomState *rng) {
    return (double)next_u32(rng) / 4294967296.0; // Divide by 2^32
}

// Singleton API
void global_random_init(const uint64_t seed) {
    random_init(&g_global_rng, seed);
}

int global_random_int(const int min, const int max) {
    return random_int(&g_global_rng, min, max);
}

double global_random_double(void) {
    return random_double(&g_global_rng);
}
