#include "tabu_search.h"
#include <stddef.h>
#include <stdlib.h>

#include "c_util.h"


union TspExtendedAlgorithms {
    TabuSearch *tabu_search;
};

static void free_this(const TspAlgorithm *self) {
    if (!self) return;

    if (self->extended) {
        if (self->extended->tabu_search) {
            if (self->extended->tabu_search->tabu_list) free((void *) self->extended->tabu_search->tabu_list);
            free(self->extended->tabu_search);
        }
        free(self->extended);
    }
    free((void *) self);
}

static void solve(const TspAlgorithm *tsp_algorithm,
                  int tour[],
                  const int number_of_nodes,
                  const double edge_cost_array[],
                  double *cost) {
    //TODO
}

const TspAlgorithm *init_tabu(const int tenure, const double time_limit) {
    int *list = malloc(sizeof(int) * tenure);
    const TabuSearch tabu_search = {.tenure = tenure, .time_limit = time_limit, .tabu_list = list};
    const TspExtendedAlgorithms extended_algorithms = {
        .tabu_search = malloc_from_stack(&tabu_search, sizeof(tabu_search))
    };
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
