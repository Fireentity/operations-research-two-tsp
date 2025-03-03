#ifndef PARSING_UTIL_H
#define PARSING_UTIL_H

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

ParsingResult parse_flag(const char *flag, const char *value, TspParams *params);

#endif
