#ifndef C_UTIL_H
#define C_UTIL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "random.h"

/* public utility functions implemented in c_util.c */
void check_alloc(const void *ptr);

void check_popen(FILE *gp);

void check_pclose(int status);

void *memdup(const void *obj, size_t size);

void join_path(char *out, const char *dir, const char *file, size_t maxlen);

void str_trim(char *s);


/* array equality macros */
#define DEFINE_ARRAYS_EQUAL(type, suffix, check)                        \
static inline bool suffix##_arrays_equal(const type arr1[], const type arr2[], size_t size) { \
    for (size_t i = 0; i < size; i++) {                                \
        type a = arr1[i];                                              \
        type b = arr2[i];                                              \
        if (!(check)) return false;                                    \
    }                                                                   \
    return true;                                                        \
}

DEFINE_ARRAYS_EQUAL(int, int, a==b)
DEFINE_ARRAYS_EQUAL(long, long, a==b)
DEFINE_ARRAYS_EQUAL(float, float, a==b)
DEFINE_ARRAYS_EQUAL(double, double, a==b)


static inline void swap_int(int *a, int *b) {
    const int tmp = *a;
    *a = *b;
    *b = tmp;
}

/* swap macros */
#define DEFINE_SWAP(type, suffix)                                   \
static inline void swap_array_##suffix(type arr[], size_t i, size_t j) {  \
    type tmp = arr[i];                                              \
    arr[i] = arr[j];                                                \
    arr[j] = tmp;                                                   \
}

DEFINE_SWAP(int, int)
DEFINE_SWAP(double, double)
DEFINE_SWAP(char, char)
DEFINE_SWAP(float, float)

/* reverse macros */
#define DEFINE_REVERSE_ARRAY(type, suffix)                                          \
static inline void reverse_array_##suffix(type arr[], size_t start, size_t end) {   \
    while (start < end) {                                                           \
        type tmp = arr[start];                                                      \
        arr[start] = arr[end];                                                      \
        arr[end] = tmp;                                                             \
        start++;                                                                    \
        end--;                                                                      \
    }                                                                               \
}

DEFINE_REVERSE_ARRAY(int, int)
DEFINE_REVERSE_ARRAY(double, double)
DEFINE_REVERSE_ARRAY(float, float)

/* shuffle macros */
#define DEFINE_SHUFFLE_ARRAY(type, suffix)                                              \
static inline void shuffle_##suffix##_array(type arr[], size_t n, RandomState *rng) {   \
    if(n < 2) return;                                                                   \
    for (size_t i = n - 1; i > 0; i--) {                                                \
        size_t j = random_int(rng, 0, i);                                               \
        type tmp = arr[i];                                                              \
        arr[i] = arr[j];                                                                \
        arr[j] = tmp;                                                                   \
    }                                                                                   \
}

DEFINE_SHUFFLE_ARRAY(int, int)
#endif
