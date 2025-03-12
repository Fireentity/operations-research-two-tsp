#ifndef FLAG_H
#define FLAG_H
#include <stdbool.h>
#include <parsing_result.h>
#include <parsing_util.h>

typedef struct FlagState FlagState;

typedef struct Flag Flag;

struct Flag
{
    FlagState* state;
    ParsingResult (*const parse)(const Flag* flag,
                                 CmdOptions* cmd_options,
                                 const char** argv,
                                 unsigned int* index);
    bool (* const is_mandatory)(const Flag* self);
    int (* const get_number_of_params)(const Flag* self);
};

const Flag* init_flag(const char* label,
                      unsigned int number_of_params,
                      ParsingResult (*param_function)(CmdOptions* cmd_options, const char** arg),
                      bool mandatory);

#endif //FLAG_H
