#include "costs_plotter.h"

#include <plot_util.h>
#include <stdlib.h>
#include "c_util.h"

struct PlotterState {
    size_t index; // index of the lowerest free cell
    size_t capacity;
    double *costs;

    void (*const free)(PlotterState *self);
};


static void free_costs_plotter(CostsPlotter *self) {
    if (!self) return;
    if (self->state) {
        self->state->free(self->state);
    }
    free(self);
}

static void free_plotter_state(PlotterState *self) {
    if (!self) return;
    if (self->costs) {
        free(self->costs);
    }
    free(self);
}

static void add_cost(const CostsPlotter *plotter, const double cost) {
    if (plotter->state->index >= plotter->state->capacity) {
        plotter->state->capacity *= 2;
        plotter->state->costs = realloc(plotter->state->costs, sizeof(double) * plotter->state->capacity);
        check_alloc(plotter->state->costs);
    }
    plotter->state->costs[plotter->state->index++] = cost;
}

static void plot_costs(const CostsPlotter *plotter, const char *file_name) {
    plot_costs_evolution(plotter->state->costs, plotter->state->index, file_name);
}

CostsPlotter *init_plotter(const size_t capacity) {
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
        .plot_costs = plot_costs
    };
    return malloc_from_stack(&stack, sizeof(stack));
}
