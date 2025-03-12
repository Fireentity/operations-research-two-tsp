#ifndef C_UTIL_H
#define C_UTIL_H
#include <stdio.h>

void check_alloc(const void *ptr);
void check_popen(FILE *gp);
void check_pclose(int status);

#define SWAP(a, b) do { __typeof__(a) _tmp = (a); (a) = (b); (b) = _tmp; } while (0)

#define REVERSE_ARRAY(arr, start, end)                  \
for (size_t i = (start), j = (end); i < j; i++, j--) {  \
    SWAP((arr)[i], (arr)[j]);                           \
}

#define SHUFFLE_ARRAY(arr, n)               \
for (size_t i = (n) - 1; i > 0; i--) {      \
    size_t j = rand() % (i + 1);            \
    SWAP((arr)[i], (arr)[j]);               \
}

#define MALLOC_FROM_STACK(obj) ({                    \
    __typeof__(obj) _tmp = (obj);                   \
    __typeof__(obj)* _ptr = malloc(sizeof(_tmp));   \
    check_alloc(_ptr);                              \
    memcpy(_ptr, &_tmp, sizeof(_tmp));              \
    _ptr;                                           \
})

#define COUNT_IF(arr, len, condition) ({    \
    int count = 0;                          \
    for (int i = 0; i < (len); i++) {       \
        if (condition) count++;             \
    }                                       \
    count;                                  \
})

#define EXECUTE_AFTER(start_time, seconds, action) do { \
    if (second() - start_time >= seconds) {  \
        printf("aaaaaaaaa");\
        action;                                         \
    }                                                   \
} while (0)

#endif //C_UTIL_H

