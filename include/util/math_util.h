//
// Created by croce on 07/03/2025.
//

#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include "tsp_instance.h"

double calculate_tour_cost(const unsigned long *const tour, const unsigned long number_of_nodes, const double *const edge_cost_array);
typedef struct {
    double min_x, max_x;
    double min_y, max_y;
} Bounds;
Bounds calculate_plot_bounds(const unsigned long *tour, const TspInstance *instance);

#define SWAP(a, b) do { __typeof__(a) _tmp = (a); (a) = (b); (b) = _tmp; } while (0)

#define MIN_IDX(arr, start, n) ({               \
int _min_idx = (start);                         \
for (int _i = (start) + 1; _i < (n); _i++) {    \
    if ((arr)[_i] < (arr)[_min_idx])            \
        _min_idx = _i;                          \
    }                                           \
    _min_idx;                                   \
})

#endif //MATH_UTIL_H
