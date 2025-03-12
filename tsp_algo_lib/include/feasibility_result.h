#ifndef FEASIBILITY_RESULT_H
#define FEASIBILITY_RESULT_H

typedef enum
{
    DUPLICATED_ENTRY,
    UNINITIALIZED_ENTRY,
    NON_MATCHING_COST,
    FEASIBLE,
} FeasibilityResult;
const char* feasibility_result_to_string(FeasibilityResult result);

#endif //FEASIBILITY_RESULT_H
