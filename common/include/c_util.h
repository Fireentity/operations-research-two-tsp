#ifndef C_UTIL_H
#define C_UTIL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* public utility functions implemented in c_util.c */
void check_alloc(const void *ptr);

void check_popen(FILE * gp);

void check_pclose(int status);

void *memdup(const void *obj, size_t size);

void join_path(char *out, const char *dir, const char *file, size_t maxlen);

void str_trim(char *s);

/* inline utility: stays in header */
static inline const char *str_file_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return NULL;
    return dot + 1;
}

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

/* random non-contiguous extraction (static → moved to .c) */
void rand_k_non_contiguous(int low, int high, int k, int result[]);

/* swap macros */
#define DEFINE_SWAP(type, suffix)                                   \
static inline void swap_##suffix(type arr[], size_t i, size_t j) {  \
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

/* copy array macros */
#define DEFINE_COPY_ARRAY(type, suffix)                                                       \
static inline void copy_##suffix##_array(const type src[], type dest[], size_t count) {       \
    for (size_t i = 0; i < count; i++) dest[i] = src[i];                                       \
}

DEFINE_COPY_ARRAY(int, int)
DEFINE_COPY_ARRAY(double, double)
DEFINE_COPY_ARRAY(float, float)

/* shuffle macros */
#define DEFINE_SHUFFLE_ARRAY(type, suffix)                     \
static inline void shuffle_##suffix##_array(type arr[], size_t n) {  \
    if(n < 2) return;                                          \
    for (size_t i = n - 1; i > 0; i--) {                       \
        size_t j = rand() % (i + 1);                           \
        type tmp = arr[i];                                     \
        arr[i] = arr[j];                                       \
        arr[j] = tmp;                                          \
    }                                                          \
}

DEFINE_SHUFFLE_ARRAY(int, int)
DEFINE_SHUFFLE_ARRAY(double, double)
DEFINE_SHUFFLE_ARRAY(float, float)

/* count-if macro */
#define DEFINE_COUNT_IF(type, suffix)                               \
static inline int count_##suffix##_if(const type arr[], size_t len, \
    bool (*predicate)(const type* arr, const type)) {               \
        int count = 0;                                              \
        for (size_t i = 0; i < len; i++)                            \
            if (predicate(arr, arr[i])) count++;                    \
        return count;                                               \
}

DEFINE_COUNT_IF(int, int)
DEFINE_COUNT_IF(double, double)
DEFINE_COUNT_IF(float, float)
DEFINE_COUNT_IF(char, char)

/* EXECUTE_AFTER macro: stays in header */
#define EXECUTE_AFTER(start_time, seconds, action) \
do {                                               \
    if (second() - start_time >= seconds) {        \
        action;                                    \
    }                                              \
} while (0)

#endif