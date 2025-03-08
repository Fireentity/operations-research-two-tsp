#ifndef FLAG_H
#define FLAG_H
#include <stdbool.h>
#include "single_flag.h"
#include "empty_flag.h"
#include "enums.h"

struct Flag
{
    union
    {
        EmptyFlag *empty_flag;
        SingleFlag *single_flag;
    };
    const char* label;
    const bool mandatory;
    ParsingResult (* const parse)(const Flag* param, const char** argv, TspParams* params, int* index);
};


#endif //FLAG_H
