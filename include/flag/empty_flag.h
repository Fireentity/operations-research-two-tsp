#ifndef EMPTY_FLAG_H
#define EMPTY_FLAG_H
#include "enums.h"
#include "cmd_options.h"

typedef Flag Flag; // forward declaration

typedef struct
{
    ParsingResult (*const param_supplier)();
} EmptyFlag;

const Flag* init_empty_flag(const char* label,
                            ParsingResult (*param_supplier)(),
                            bool mandatory);

#endif //EMPTY_FLAG_H
