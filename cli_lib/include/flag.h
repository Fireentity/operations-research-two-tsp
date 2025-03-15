#ifndef FLAG_H
#define FLAG_H

#include <stdbool.h>
#include <parsing_result.h>
#include <parsing_util.h>
#include <stdint-gcc.h>

typedef struct FlagState FlagState;

typedef struct Flag Flag;


struct Flag {
    FlagState *state;

    ParsingResult (*const parse)(const Flag *flag,
                                 CmdOptions *cmd_options,
                                 const char **argv,
                                 unsigned int *index);

    bool (*const is_mandatory)(const Flag *self);

    unsigned int (*const get_number_of_params)(const Flag *self);

    const char* (*const get_label)(const Flag *self);
};

const Flag *init_flag(const char *label,
                      unsigned int number_of_params,
                      ParsingResult (*param_function)(CmdOptions *cmd_options, const char **arg),
                      bool mandatory);

uint64_t hash(const void *item, uint64_t seed0, uint64_t seed1);
int compare(const void *a, const void *b, void *udata);
void free_flag(void *item);


#endif //FLAG_H
