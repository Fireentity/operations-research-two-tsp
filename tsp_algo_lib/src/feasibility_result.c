#include <feasibility_result.h>

const char* feasibility_result_to_string(const FeasibilityResult result)
{
    switch (result)
    {
    case DUPLICATED_ENTRY: return "DUPLICATED_ENTRY";
    case UNINITIALIZED_ENTRY: return "UNINITIALIZED_ENTRY";
    case NON_MATCHING_COST: return "NON_MATCHING_COST";
    case FEASIBLE: return "FEASIBLE";
    default: return "UNKNOWN_FEASIBILITY_RESULT";
    }
}
