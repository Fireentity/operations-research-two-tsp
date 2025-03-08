#ifndef EMPTY_FLAG_H
#define EMPTY_FLAG_H
#include "enums.h"
#include "tsp_params.h"

typedef Flag Flag; // forward declaration

typedef struct
{
    ParsingResult (*const param_supplier)(TspParams* self);
} EmptyFlag;

const Flag* init_empty_flag(const char* label,
                            ParsingResult (*param_supplier)(TspParams* self),
                            bool mandatory);

#endif //EMPTY_FLAG_H
