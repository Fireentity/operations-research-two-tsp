#ifndef COSTS_PLOTTER_H
#define COSTS_PLOTTER_H
#include <stddef.h>

typedef struct PlotterState PlotterState;
typedef struct CostsPlotter CostsPlotter;

struct CostsPlotter {
    PlotterState *state;

    void (*const free)(const CostsPlotter *self);

    void (*const add_cost)(const CostsPlotter *self, double cost);

    void (*const plot_costs)(const CostsPlotter *self, const char *file_name);
};

CostsPlotter *init_plotter(size_t capacity);

#endif //COSTS_PLOTTER_H
