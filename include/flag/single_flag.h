#ifndef SINGLEARGFLAG_H
#define SINGLEARGFLAG_H
#include "enums.h"
#include "cmd_options.h"

typedef struct Flag Flag; // forward declaration

typedef struct
{
    ParsingResult (*const param_supplier)(const char* arg);
} SingleFlag;

const Flag* init_single_flag(const char* label,
                             ParsingResult (*param_supplier)(const char* arg),
                             bool mandatory
);

#endif //SINGLEARGFLAG_H
