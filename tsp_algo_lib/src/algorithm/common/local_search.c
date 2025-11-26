#include "constants.h"
#include "c_util.h"
#include "logger.h"
#include <stdbool.h>
#include "time_limiter.h"

double two_opt(int *tour,
                      const int number_of_nodes,
                      const double *edge_cost_array,
                      const TimeLimiter timer) {
    double cost_improvement = 0;
    bool improved = true;

    while (improved) {
        improved = false;

        for (int i = 1; i < number_of_nodes - 1; i++) {
            if (time_limiter_is_over(&timer)) {
                if_verbose(VERBOSE_DEBUG, "  2-Opt: Time limit reached during optimization. Total improvement: %lf\n",
                           cost_improvement);
                return cost_improvement;
            }
            for (int j = i + 1; j < number_of_nodes; j++) {
                if (i == 1 && j == number_of_nodes - 1)
                    continue;

                const int a = tour[i - 1];
                const int b = tour[i];
                const int c = tour[j];
                const int d = tour[j + 1];

                const double delta = edge_cost_array[a * number_of_nodes + c] +
                                     edge_cost_array[b * number_of_nodes + d] -
                                     (edge_cost_array[a * number_of_nodes + b] +
                                      edge_cost_array[c * number_of_nodes + d]);

                if (delta < -EPSILON) {
                    cost_improvement += delta;
                    reverse_array_int(tour, i, j);
                    improved = true;
                    break; // Break inner loop
                }
            }
            if (improved) break; // Break outer loop to restart from i = 1
        }
        // If no improvement was found in the full pass, the outer loop will terminate
    }

    if_verbose(VERBOSE_ALL, "  2-Opt: Finished local search. Total improvement: %lf\n", cost_improvement);
    return cost_improvement;
}
