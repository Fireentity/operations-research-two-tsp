#include "costs_plotter.h"
#include "plot_util.h"
#include "c_util.h"    // check_alloc
#include "logger.h"    // if_verbose
#include <stdlib.h>

// Flattened struct: no "State" indirection needed
struct CostsPlotter {
    double *costs;
    int count;
    int capacity;
};

CostsPlotter *costs_plotter_create(int initial_capacity) {
    if (initial_capacity <= 0) initial_capacity = 100; // Safe default

    CostsPlotter *plotter = malloc(sizeof(CostsPlotter));
    check_alloc(plotter);

    plotter->costs = malloc(initial_capacity * sizeof(double));
    check_alloc(plotter->costs);

    plotter->capacity = initial_capacity;
    plotter->count = 0;

    return plotter;
}

void costs_plotter_destroy(CostsPlotter *plotter) {
    if (!plotter) return;

    if (plotter->costs) {
        free(plotter->costs);
    }
    free(plotter);
}

void costs_plotter_add(CostsPlotter *plotter, double cost) {
    if (!plotter) return;

    // Resize strategy: Double capacity when full
    if (plotter->count >= plotter->capacity) {
        int new_capacity = plotter->capacity * 2;
        double *new_costs = realloc(plotter->costs, new_capacity * sizeof(double));
        check_alloc(new_costs);

        plotter->costs = new_costs;
        plotter->capacity = new_capacity;
    }

    plotter->costs[plotter->count++] = cost;
}

void costs_plotter_plot(const CostsPlotter *plotter, const char *filename) {
    if (!plotter || plotter->count == 0) {
        if_verbose(VERBOSE_DEBUG, "Skipping plot '%s': no data.\n", filename ? filename : "(null)");
        return;
    }

    if_verbose(VERBOSE_DEBUG, "Plotting cost evolution (%d points) to '%s'...\n",
               plotter->count, filename);

    // Delegate to the low-level utility
    plot_costs_evolution(plotter->costs, plotter->count, filename);
}
