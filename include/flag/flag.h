#ifndef FLAG_H
#define FLAG_H
#include <stdbool.h>
#include "enums.h"

typedef struct Flag Flag;

struct Flag {
    const unsigned int number_of_params;
    const char *label;
    const bool mandatory;
    ParsingResult (* const parse_fuction)(const char **arg);
};

ParsingResult parse(const Flag* flag, const char** argv, int* index);

const Flag* init_flag(const char* label,
                      unsigned int number_of_params,
                      ParsingResult (*param_supplier)(const char** arg),
                      bool mandatory
);

#endif //FLAG_H
