#ifndef SINGLEARGFLAG_H
#define SINGLEARGFLAG_H
#include "enums.h"
#include "tsp_params.h"

typedef struct Flag Flag; // forward declaration

typedef struct
{
    ParsingResult (*const param_supplier)(TspParams* self, const char* arg);
} SingleFlag;

const Flag* init_single_flag(const char* label,
                             ParsingResult (*param_supplier)(TspParams* self, const char* arg),
                             bool mandatory
);

#endif //SINGLEARGFLAG_H
