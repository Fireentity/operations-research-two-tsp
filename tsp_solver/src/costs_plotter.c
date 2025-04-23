#include "costs_plotter.h"
#include "algorithm_plotter.h"
#include <stdlib.h>
#include "c_util.h"
#include "plot_util.h"

struct PlotterState {
    int index; // index of the lowermost free cell
    int capacity;
    double *costs;

    void (*const free)(PlotterState *self);
};


static void free_costs_plotter(const CostsPlotter *self) {
    if (!self) return;
    if (self->state) {
        self->state->free(self->state);
    }
    free((void*)self);
}

static void free_plotter_state(PlotterState *self) {
    if (!self) return;
    if (self->costs) {
        free(self->costs);
    }
    free(self);
}

static void add_cost(const CostsPlotter *self, const double cost) {
    if (self->state->index >= self->state->capacity) {
        self->state->capacity *= 2;
        self->state->costs = realloc(self->state->costs, sizeof(double) * self->state->capacity);
        check_alloc(self->state->costs);
    }
    self->state->costs[self->state->index++] = cost;
}

static void plot_costs(const CostsPlotter *self, const char *file_name) {
    plot_costs_evolution(self->state->costs, self->state->index, file_name);
}

CostsPlotter *init_plotter(const int capacity) {
    double *costs = malloc(sizeof(double) * capacity);
    check_alloc(costs);
    const PlotterState state = {
        .capacity = capacity,
        .costs = costs,
        .index = 0,
        .free = free_plotter_state
    };
    const CostsPlotter stack = {
        .state = malloc_from_stack(&state, sizeof(state)),
        .free = free_costs_plotter,
        .add_cost = add_cost,
        .plot = plot_costs
    };
    return malloc_from_stack(&stack, sizeof(stack));
}
