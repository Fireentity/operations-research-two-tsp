#ifndef TSP_PARSER_UTILS_H
#define TSP_PARSER_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

static inline int read_next_token(FILE *f, char *buf, size_t sz) {
    if (!f || !buf || sz == 0) return -1;

    int c;
    while ((c = fgetc(f)) != EOF && isspace((unsigned char)c));
    if (c == EOF) return -1;

    size_t i = 0;
    if (i < sz - 1) buf[i++] = (char)c;

    while ((c = fgetc(f)) != EOF && !isspace((unsigned char)c)) {
        if (i < sz - 1) buf[i++] = (char)c;
    }
    buf[i] = '\0';
    return 0;
}

static inline int parse_int(const char *s, int *out) {
    errno = 0;
    char *end;
    long v = strtol(s, &end, 10);

    if (errno == ERANGE || v < INT_MIN || v > INT_MAX) return -2;
    while (*end && isspace((unsigned char)*end)) end++;
    if (*end != '\0') return -1;

    *out = (int)v;
    return 0;
}

static inline int parse_double(const char *s, double *out) {
    errno = 0;
    char *end;
    double v = strtod(s, &end);

    if (errno == ERANGE) return -2;
    while (*end && isspace((unsigned char)*end)) end++;
    if (*end != '\0') return -1;

    *out = v;
    return 0;
}

#endif
