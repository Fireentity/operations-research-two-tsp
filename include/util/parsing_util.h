#ifndef PARSING_UTIL_H
#define PARSING_UTIL_H
#include <stdbool.h>

#include "enums.h"

ParsingResult parse_flag(const Flag* flag,
                         const char** argv,
                         int* index);
ParsingResult parse_unsigned_int(const char* arg, unsigned int* parsed);
ParsingResult parse_int(const char* arg, int* parsed);
ParsingResult parse_float(const char* arg, float* parsed);
ParsingResult parse_bool(const char* arg, bool* parsed);

#endif //PARSING_UTIL_H
