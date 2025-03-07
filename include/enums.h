#ifndef ENUMS_H
#define ENUMS_H

static const char*const feasibility_result_map[];

static const char*const parsing_result_map[];

typedef enum
{
    SUCCESS = 0,
    PARSING_ERROR = 1,
    UNKNOWN_ARG = 2,
} ParsingResult;

typedef enum
{
    DUPLICATED_ENTRY = 0,
    UNINITIALIZED_ENTRY = 1,
    NON_MATCHING_COST = 2,
    FEASIBLE = 3,
} FeasibilityResult;

#define ENUM_TO_STRING(x) _Generic((x), \
ParsingResult: parsing_result_map[(x)], \
FeasibilityResult: feasibility_result_map[(x)] )

#endif //ENUMS_H
