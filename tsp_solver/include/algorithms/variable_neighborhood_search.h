#ifndef VARIABLE_NEIGHBORHOOD_SEARCH_H
#define VARIABLE_NEIGHBORHOOD_SEARCH_H

/**
 * @brief Configuration for the Variable Neighborhood Search algorithm.
 */
typedef struct
{
    const int kick_repetition; /**< Number of kick repetitions. */
    const double time_limit;   /**< Maximum allowed time for the search. */
} VariableNeighborhoodSearch;

#endif //VARIABLE_NEIGHBORHOOD_SEARCH_H
