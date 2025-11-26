#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H
#include "time_limiter.h"

double two_opt(int *tour,
               int number_of_nodes,
               const double *edge_cost_array,
               TimeLimiter timer);

#endif //LOCAL_SEARCH_H