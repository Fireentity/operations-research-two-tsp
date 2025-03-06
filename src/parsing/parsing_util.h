#ifndef PARSING_UTIL_H
#define PARSING_UTIL_H

#include <stdbool.h>

typedef enum {
    SUCCESS,
    PARSING_ERROR,
    UNKNOWN_ARG
} ParsingResult;

typedef struct {
    long x_square;
    long y_square;
    long square_side;
} Rectangle;

typedef struct {
    long number_of_nodes;
    long seed;
    Rectangle rectangle;
    long help;
} TspParams;

typedef struct {
    const char *label;
    ParsingResult (*parse)(const char * arg, TspParams *params);
} CommandLineFlag;

ParsingResult parse_flag(const char *flag, const char *value, TspParams *params);
ParsingResult parse_long(const char *arg,long* parsed);
ParsingResult parse_float(const char *arg, float* parsed);
ParsingResult parse_bool(const char *arg, bool* parsed);

#endif
