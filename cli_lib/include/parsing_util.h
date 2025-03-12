#ifndef PARSING_UTIL_H
#define PARSING_UTIL_H

#include <stdbool.h>
#include <parsing_result.h>

typedef struct Flag Flag;

typedef struct CmdOptions CmdOptions;

ParsingResult parse_unsigned_int(const char* arg, unsigned int* parsed);
ParsingResult parse_int(const char* arg, int* parsed);
ParsingResult parse_float(const char* arg, float* parsed);
ParsingResult parse_bool(const char* arg, bool* parsed);

ParsingResult parse_flags(CmdOptions* cmd_options,
                 const Flag** tsp_flags,
                 int number_of_flags,
                 int argc,
                 const char** argv);

#define PARSING_ABORT(result) do { \
    printf("Command line parsing, reason : %s\n", parsing_result_to_string(result)); \
    exit(EXIT_FAILURE); \
} while(0)

#endif //PARSING_UTIL_H
