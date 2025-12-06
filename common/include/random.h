#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

/**
 * @brief State of the random number generator.
 */
typedef struct {
    uint64_t state;
} RandomState;

/**
 * @brief Initializes the generator with a seed.
 */
void random_init(RandomState *rng, uint64_t seed);

/**
 * @brief Returns a random integer in the range [min, max].
 */
int random_int(RandomState *rng, int min, int max);

/**
 * @brief Returns a random double in [0.0, 1.0).
 */
double random_double(RandomState *rng);

/**
 * @brief Initializes the global generator (not thread-safe).
 */
void global_random_init(uint64_t seed);

/**
 * @brief Returns a random integer using the global generator.
 */
int global_random_int(int min, int max);

/**
 * @brief Returns a random double using the global generator.
 */
double global_random_double(void);

#endif
