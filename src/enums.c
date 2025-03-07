#include "enums.h"

static const char*const feasibility_result_map[] ={
    "DUPLICATED_ENTRY",
    "UNINITIALIZED_ENTRY",
    "NON_MATCHING_COST",
    "FEASIBLE"
};

static const char*const parsing_result_map[] ={
    "SUCCESS",
    "PARSING_ERROR",
    "UNKNOWN_ARG",
};