#ifndef FEASIBILITY_RESULT_H
#define FEASIBILITY_RESULT_H

/**
 * @brief Enumeration of feasibility results.
 */
typedef enum {
    DUPLICATED_ENTRY, /**< Entry is duplicated. */
    UNINITIALIZED_ENTRY, /**< Entry is uninitialized. */
    NON_MATCHING_COST, /**< Cost does not match expected value. */
    FEASIBLE, /**< Result is feasible. */
} FeasibilityResult;

/**
 * @brief Converts a FeasibilityResult to its string representation.
 *
 * @param result The feasibility result.
 * @return A string corresponding to the feasibility result.
 */
const char *feasibility_result_to_string(FeasibilityResult result);

#endif //FEASIBILITY_RESULT_H
