#ifndef COSTS_PLOTTER_H
#define COSTS_PLOTTER_H

typedef struct PlotterState PlotterState;
typedef struct CostsPlotter CostsPlotter;

struct CostsPlotter {
    PlotterState *state;

    void (*const free)(const CostsPlotter *self);

    void (*const add_cost)(const CostsPlotter *self, double cost);

    void (*const plot)(const CostsPlotter *self, const char *file_name);
};

#endif //COSTS_PLOTTER_H
