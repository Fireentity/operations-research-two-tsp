#ifndef PARSING_UTIL_H
#define PARSING_UTIL_H

#include <stdbool.h>

#include "tsp_params.h"

typedef enum
{
    SUCCESS,
    PARSING_ERROR,
    UNKNOWN_ARG
} ParsingResult;

typedef struct
{
    const char* label;
    const ParsingResult (*parse)(const char* arg, TspParams* params);
    const bool mandatory;
} TspParam;

ParsingResult parse_flag(const char* flag, const char* value, TspParams* params);
ParsingResult parse_long(const char* arg, long* parsed);
ParsingResult parse_float(const char* arg, float* parsed);
ParsingResult parse_bool(const char* arg, bool* parsed);

#endif
