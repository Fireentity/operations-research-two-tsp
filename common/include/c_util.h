#ifndef C_UTIL_H
#define C_UTIL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "random.h"

//If TSP_DEBUG is defined, we use the tracking wrappers, else we map directly to standard functions for zero overhead.

#if defined(TSP_DEBUG) || defined(DEBUG)

    #define tsp_malloc(size)       _tsp_malloc_dbg(size, __FILE__, __LINE__)
    #define tsp_calloc(num, size)  _tsp_calloc_dbg(num, size, __FILE__, __LINE__)
    #define tsp_realloc(ptr, size) _tsp_realloc_dbg(ptr, size, __FILE__, __LINE__)
    #define tsp_free(ptr)          _tsp_free_dbg(ptr)

    /* Internal debug functions - Do not call directly */
    void* _tsp_malloc_dbg(size_t size, const char* file, int line);
    void* _tsp_calloc_dbg(size_t num, size_t size, const char* file, int line);
    void* _tsp_realloc_dbg(void* ptr, size_t size, const char* file, int line);
    void  _tsp_free_dbg(void* ptr);

    /**
     * @brief Prints a report of currently allocated memory (leaks).
     */
    void tsp_dump_memory_leaks(void);

#else

    /* Release Mode: Direct mapping with error checking */
    static inline void* tsp_malloc(size_t size) {
        void* ptr = malloc(size);
        if (!ptr && size > 0) { perror("malloc failed"); exit(EXIT_FAILURE); }
        return ptr;
    }

    static inline void* tsp_calloc(size_t num, size_t size) {
        void* ptr = calloc(num, size);
        if (!ptr && (num * size) > 0) { perror("calloc failed"); exit(EXIT_FAILURE); }
        return ptr;
    }

    static inline void* tsp_realloc(void* ptr, size_t size) {
        void* new_ptr = realloc(ptr, size);
        if (!new_ptr && size > 0) { perror("realloc failed"); exit(EXIT_FAILURE); }
        return new_ptr;
    }

    #define tsp_free(ptr) free(ptr)
    #define tsp_dump_memory_leaks() ((void)0)

#endif

/* Existing utilities */
void check_alloc(const void *ptr); // Can be deprecated in favor of tsp_malloc
void check_popen(FILE *gp);
void check_pclose(int status);
void *memdup(const void *obj, size_t size);
void join_path(char *out, const char *dir, const char *file, size_t maxlen);
void str_trim(char *s);

/* array equality macros */
#define DEFINE_ARRAYS_EQUAL(type, suffix, check) \
static inline bool suffix##_arrays_equal(const type arr1[], const type arr2[], size_t size) { \
    for (size_t i = 0; i < size; i++) { \
        type a = arr1[i]; \
        type b = arr2[i]; \
        if (!(check)) return false; \
    } \
    return true; \
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

#define DEFINE_SWAP(type, suffix) \
static inline void swap_array_##suffix(type arr[], size_t i, size_t j) { \
    type tmp = arr[i]; \
    arr[i] = arr[j]; \
    arr[j] = tmp; \
}

DEFINE_SWAP(int, int)
DEFINE_SWAP(double, double)
DEFINE_SWAP(char, char)
DEFINE_SWAP(float, float)

#define DEFINE_REVERSE_ARRAY(type, suffix) \
static inline void reverse_array_##suffix(type arr[], size_t start, size_t end) { \
    while (start < end) { \
        type tmp = arr[start]; \
        arr[start] = arr[end]; \
        arr[end] = tmp; \
        start++; \
        end--; \
    } \
}

DEFINE_REVERSE_ARRAY(int, int)
DEFINE_REVERSE_ARRAY(double, double)
DEFINE_REVERSE_ARRAY(float, float)

#define DEFINE_SHUFFLE_ARRAY(type, suffix) \
static inline void shuffle_##suffix##_array(type arr[], size_t n, RandomState *rng) { \
    if(n < 2) return; \
    for (size_t i = n - 1; i > 0; i--) { \
        size_t j = random_int(rng, 0, i); \
        type tmp = arr[i]; \
        arr[i] = arr[j]; \
        arr[j] = tmp; \
    } \
}

DEFINE_SHUFFLE_ARRAY(int, int)

#endif // C_UTIL_H