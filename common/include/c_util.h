#ifndef C_UTIL_H
#define C_UTIL_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void check_alloc(const void* ptr)
{
    if (!ptr)
    {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
}

static inline void check_popen(FILE* gp)
{
    if (!gp)
    {
        perror("popen error");
        exit(EXIT_FAILURE);
    }
}

static inline void check_pclose(const int status)
{
    if (status == -1)
    {
        perror("pclose error");
        exit(EXIT_FAILURE);
    }
}

static inline void *malloc_from_stack(const void *obj, const size_t size) {
    void *ptr = malloc(size);
    check_alloc(ptr);
    memcpy(ptr, obj, size);
    return ptr;
}

static inline bool arrays_equal(const void *arr1, const void *arr2, const size_t size, const size_t elem_size) {
    return memcmp(arr1, arr2, size * elem_size) == 0;
}

/**
 * @brief Extracts k non-contiguous numbers from a given range.
 *
 * Generates k random numbers from [low, high] ensuring that
 * no two numbers are adjacent.
 *
 * @param low    Lower bound (inclusive).
 * @param high   Upper bound (inclusive).
 * @param k      Number of numbers to extract.
 * @param result Array to store the resulting numbers.
 */
static inline void rand_k_non_contiguous(const int low, const int high, const int k, int result[]) {
    // Calculate effective range size adjusted for required gaps.
    const int m = high - low + 1 - (k - 1);
    int s[k];  // Array to store offset values.

    // Generate the first offset.
    s[0] = rand() % (m - (k - 1));

    // Generate subsequent offsets with at least one gap between them.
    for (int i = 1; i < k; i++)
        s[i] = s[i - 1] + 1 + rand() % (m - s[i - 1] - (k - i));

    // Adjust offsets to produce final non-contiguous numbers.
    for (int i = 0; i < k; i++)
        result[i] = low + s[i] + i;
}

#define DEFINE_SWAP(type, suffix)                                   \
static inline void swap_##suffix(type arr[], size_t i, size_t j) {  \
    type tmp = arr[i];                                              \
    arr[i] = arr[j];                                                \
    arr[j] = tmp;                                                   \
}

DEFINE_SWAP(int, int);
DEFINE_SWAP(double, double);
DEFINE_SWAP(char, char);
DEFINE_SWAP(float, float);

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

#define DEFINE_SHUFFLE_ARRAY(type, suffix)                     \
static inline void shuffle_##suffix##_array(type arr[], size_t n) {\
    if(n < 2) return;                                          \
        for (size_t i = n - 1; i > 0; i--) {                         \
            size_t j = rand() % (i + 1);                             \
            type tmp = arr[i];                                     \
            arr[i] = arr[j];                                       \
            arr[j] = tmp;                                          \
    }                                                          \
}

DEFINE_SHUFFLE_ARRAY(int, int)
DEFINE_SHUFFLE_ARRAY(double, double)
DEFINE_SHUFFLE_ARRAY(float, float)

#define DEFINE_COUNT_IF(type, suffix)                               \
static inline int count_##suffix##_if(const type arr[], size_t len,     \
    bool (*predicate)(const type* arr, const type)) {   \
            int count = 0;                                                  \
            for (size_t i = 0; i < len; i++) {                                \
            if (predicate(arr, arr[i])) count++;                             \
        }                                                               \
    return count;                                                   \
}

DEFINE_COUNT_IF(int, int)
DEFINE_COUNT_IF(double, double)
DEFINE_COUNT_IF(float, float)
DEFINE_COUNT_IF(char, char)

#define EXECUTE_AFTER(start_time, seconds, action) do { \
    if (second() - start_time >= seconds) {  \
        printf("aaaaaaaaa");\
        action;                                         \
    }                                                   \
} while (0)
#endif //C_UTIL_H

