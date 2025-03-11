#ifndef FLAG_H
#define FLAG_H
#include <cmd_options.h>
#include <stdbool.h>
#include <parsing_result.h>

typedef struct {
    const unsigned int number_of_params;
    const char *label;
    const bool mandatory;
    ParsingResult (* const parse_function)(const CmdOptions *cmd_options, const char **arg);
} Flag;

ParsingResult parse(const Flag* flag, const CmdOptions* cmd_options, const char** argv, int* index);

const Flag* init_flag(const char* label,
                      unsigned int number_of_params,
                      ParsingResult (*param_supplier)(const char** arg),
                      bool mandatory
);

#endif //FLAG_H
