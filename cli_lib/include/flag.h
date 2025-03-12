#ifndef FLAG_H
#define FLAG_H
#include <stdbool.h>
#include <parsing_result.h>

typedef struct {
    const unsigned int number_of_params;
    const char *label;
    const bool mandatory;
    ParsingResult (* const parse_function)(void *options, const char **arg);
} Flag;

ParsingResult parse(const Flag* flag,
                    void* options,
                    const char** argv,
                    unsigned int* index);

const Flag* init_flag(const char* label,
                      unsigned int number_of_params,
                      ParsingResult (*param_supplier)(void* options, const char** arg),
                      bool mandatory
);

#endif //FLAG_H
