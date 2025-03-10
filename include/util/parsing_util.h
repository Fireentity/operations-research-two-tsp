#ifndef PARSING_UTIL_H
#define PARSING_UTIL_H

#include <stdbool.h>

#include "enums.h"

ParsingResult parse_unsigned_int(const char* arg, unsigned int* parsed);
ParsingResult parse_int(const char* arg, int* parsed);
ParsingResult parse_float(const char* arg, float* parsed);
ParsingResult parse_bool(const char* arg, bool* parsed);

void parse_command_line(int argc, const char **argv);

#define PARSING_ABORT(result) do { \
    printf("Command line parsing, reason : %s\n", parsing_result_to_string(result)); \
    exit(EXIT_FAILURE); \
} while(0)

#endif //PARSING_UTIL_H
