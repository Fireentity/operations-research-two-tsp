#ifndef C_UTIL_H
#define C_UTIL_H
#include <stdio.h>
#include <stdlib.h>

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

#endif //C_UTIL_H

