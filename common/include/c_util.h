#ifndef C_UTIL_H
#define C_UTIL_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define VERBOSE_NONE  0
#define VERBOSE_INFO  1
#define VERBOSE_DEBUG 2

#ifndef DISABLE_VERBOSE
static inline void if_verbose(const unsigned int current_level, const unsigned int required_level, const char *text) {
    if (current_level >= required_level)
        printf("%s\n", text);
}
#else
#define if_verbose(current_level, required_level, text) ((void)0)
#endif

/**
 * @brief Checks if the given pointer is not NULL.
 *
 * If the pointer is NULL, prints an error and exits.
 *
 * @param ptr Pointer to check.
 */
static inline void check_alloc(const void* ptr) {
    if (!ptr) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Checks the FILE pointer returned by popen.
 *
 * If the pointer is NULL, prints an error and exits.
 *
 * @param gp FILE pointer returned by popen.
 */
static inline void check_popen(FILE* gp) {
    if (!gp) {
        perror("popen error");
        exit(EXIT_FAILURE);
    }
}

/**
 * Allocate and return a formatted string, like printf, but returns
 * a malloc’d buffer instead of writing into one you provide.
 *
 * @param format
 *   A printf-style format string (e.g. "Hello %s, you have %d new msgs")
 * @param ...
 *   The arguments to fill into fmt.
 * @return
 *   A malloc’ed C‐string containing the formatted result, or NULL on OOM.
 *   Caller is responsible for free()’ing it.
 */
static inline char* str_format(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // First, measure how long the final string will be:
    const int needed = vsnprintf(NULL, 0, format, args);
    va_end(args);

    if (needed < 0) {
        // encoding error
        return NULL;
    }

    // Allocate exactly the right size (+1 for NUL)
    char* buf = malloc((size_t)needed + 1);
    if (!buf) return NULL;

    // Actually format into our buffer
    va_start(args, format);
    vsnprintf(buf, (size_t)needed + 1, format, args);
    va_end(args);

    return buf;
}

static inline char* vstr_format(const char* format, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);

    // Figure out how big the resulting string will be
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (needed < 0) return NULL;

    // Allocate space (needed + 1 for the NUL)
    char* buf = malloc((size_t)needed + 1);
    if (!buf) return NULL;

    // Actually format into it
    vsnprintf(buf, (size_t)needed + 1, format, args);
    return buf;
}


/**
 * @brief Checks the status returned by pclose.
 *
 * If the status equals -1, prints an error and exits.
 *
 * @param status Status returned by pclose.
 */
static inline void check_pclose(const int status) {
    if (status == -1) {
        perror("pclose error");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Allocates memory and copies an object from the stack.
 *
 * Allocates memory of given size, checks allocation, and copies the object.
 *
 * @param obj Pointer to the source object.
 * @param size Size of the object in bytes.
 * @return Pointer to the newly allocated memory containing the copied object.
 */
static inline void* memdup(const void* obj, const size_t size) {
    void* ptr = malloc(size);
    check_alloc(ptr);
    memcpy(ptr, obj, size);
    return ptr;
}

/**
 * @brief Macro to define an arrays equality function for arrays.
 *
 * Generates an inline function arrays_equal_suffix that compares two arrays for equality
 * by checking each element individually.
 *
 * @param type Data type of the array elements.
 * @param suffix Suffix for the generated function name.
 */
#define DEFINE_ARRAYS_EQUAL(type, suffix, check)                        \
static inline bool suffix##_arrays_equal(const type arr1[], const type arr2[], size_t size) { \
    for (size_t i = 0; i < size; i++) {                                   \
        type a = arr1[i];                                               \
        type b = arr2[i];                                               \
        if (!(check)) {                                                 \
            return false;                                               \
        }                                                               \
    }                                                                   \
    return true;                                                        \
}

DEFINE_ARRAYS_EQUAL(int, int, a==b);
DEFINE_ARRAYS_EQUAL(long, long, a==b);
DEFINE_ARRAYS_EQUAL(float, float, a==b);
DEFINE_ARRAYS_EQUAL(double, double, a==b);


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
    int s[k]; // Array to store offset values.

    // Generate the first offset.
    s[0] = rand() % (m - (k - 1));

    // Generate subsequent offsets with at least one gap between them.
    for (int i = 1; i < k; i++)
        s[i] = s[i - 1] + 1 + rand() % (m - s[i - 1] - (k - i));

    // Adjust offsets to produce final non-contiguous numbers.
    for (int i = 0; i < k; i++)
        result[i] = low + s[i] + i;
}

/**
 * @brief Macro to define a swap function for arrays.
 *
 * Generates an inline function swap_suffix that swaps two elements in an array.
 *
 * @param type Data type of the array elements.
 * @param suffix Suffix for the generated function name.
 */
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

/**
 * @brief Macro to define a function to reverse a subarray.
 *
 * Generates an inline function reverse_array_suffix that reverses elements in an array
 * between the indices start and end.
 *
 * @param type Data type of the array elements.
 * @param suffix Suffix for the generated function name.
 */
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

/**
 * @brief Macro to define a function to copy an array.
 *
 * Generates an inline function copy_array_suffix that copies elements from a source
 * array to a destination array for a given number of elements.
 *
 * @param type Data type of the array elements.
 * @param suffix Suffix for the generated function name.
 */
#define DEFINE_COPY_ARRAY(type, suffix)                                                       \
static inline void copy_##suffix##_array(const type src[], type dest[], size_t count) {           \
    for (size_t i = 0; i < count; i++) {                                                        \
        dest[i] = src[i];  /* Copy current element from source to destination */              \
    }                                                                                           \
}

DEFINE_COPY_ARRAY(int, int)
DEFINE_COPY_ARRAY(double, double)
DEFINE_COPY_ARRAY(float, float)

/**
 * @brief Macro to define a function to shuffle an array.
 *
 * Generates an inline function shuffle_suffix_array that randomly shuffles elements in an array.
 *
 * @param type Data type of the array elements.
 * @param suffix Suffix for the generated function name.
 */
#define DEFINE_SHUFFLE_ARRAY(type, suffix)                     \
static inline void shuffle_##suffix##_array(type arr[], size_t n) {  \
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

/**
 * @brief Macro to define a function that counts elements satisfying a predicate.
 *
 * Generates an inline function count_suffix_if that iterates through an array and
 * counts the number of elements for which the predicate returns true.
 *
 * @param type Data type of the array elements.
 * @param suffix Suffix for the generated function name.
 */
#define DEFINE_COUNT_IF(type, suffix)                               \
static inline int count_##suffix##_if(const type arr[], size_t len,     \
    bool (*predicate)(const type* arr, const type)) {   \
            int count = 0;                                                  \
            for (size_t i = 0; i < len; i++) {                                \
                if (predicate(arr, arr[i])) count++;                          \
            }                                                               \
        return count;                                                   \
}

DEFINE_COUNT_IF(int, int)
DEFINE_COUNT_IF(double, double)
DEFINE_COUNT_IF(float, float)
DEFINE_COUNT_IF(char, char)

/**
 * @brief Executes an action after a specified time interval.
 *
 * If the time elapsed since start_time is at least seconds, prints a message and executes the action.
 *
 * @param start_time Starting time.
 * @param seconds Time interval threshold in seconds.
 * @param action Action to execute when the interval is reached.
 */
#define EXECUTE_AFTER(start_time, seconds, action) do { \
    if (second() - start_time >= seconds) {  \
        printf("aaaaaaaaa");\
        action;                                         \
    }                                                   \
} while (0)

#endif //C_UTIL_H
