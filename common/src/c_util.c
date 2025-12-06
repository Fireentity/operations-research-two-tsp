#include "c_util.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void check_alloc(const void *ptr) {
    if (!ptr) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
}

void check_popen(FILE *gp) {
    if (!gp) {
        perror("popen error");
        exit(EXIT_FAILURE);
    }
}

void check_pclose(const int status) {
    if (status == -1) {
        perror("pclose error");
        exit(EXIT_FAILURE);
    }
}

void *memdup(const void *obj, const size_t size) {
    void *ptr = malloc(size);
    check_alloc(ptr);
    memcpy(ptr, obj, size);
    return ptr;
}

void join_path(char *out, const char *dir, const char *file, size_t maxlen) {
    const size_t len = strlen(dir);

    if (len > 0 && dir[len - 1] == '/')
        snprintf(out, maxlen, "%s%s", dir, file);
    else
        snprintf(out, maxlen, "%s/%s", dir, file);
}

void str_trim(char *s) {
    if (!s || !*s) return;

    const char *start = s;
    while (*start && isspace((unsigned char) *start))
        start++;

    if (start != s)
        memmove(s, start, strlen(start) + 1);

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char) s[len - 1])) {
        s[len - 1] = 0;
        len--;
    }
}

