#include "grasp.h"
#include "c_util.h"

union TspExtendedAlgorithms {
    Grasp *grasp;
};

static void solve(
    const TspAlgorithm *tsp_algorithm,
    int tour[],
    const int number_of_nodes,
    const double edge_cost_array[],
    double *cost
) {

}

static void free_this(const TspAlgorithm *self) {
    if (!self) return;

    if (self->extended) {
        if (self->extended->grasp) {
            free(self->extended->grasp);
        }
        free(self->extended);
    }
    free((void *) self);
}

const TspAlgorithm *init_grasp(const double time_limit) {
    // Create a Grasp structure with given parameters.
    const Grasp grasp = {
        .time_limit = time_limit,
    };
    // Allocate and initialize extended algorithm structure.
    const TspExtendedAlgorithms extended_algorithms = {
        .grasp = malloc_from_stack(&grasp, sizeof(grasp))
    };
    // Initialize the main TspAlgorithm structure with solve and free functions.
    const TspAlgorithm tsp_algorithm = {
        .solve = solve,
        .free = free_this,
        .extended = malloc_from_stack(&extended_algorithms, sizeof(extended_algorithms)),
    };
    return malloc_from_stack(&tsp_algorithm, sizeof(tsp_algorithm));
}
